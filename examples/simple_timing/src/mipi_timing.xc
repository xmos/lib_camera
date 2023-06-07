#include <xs1.h>
#include <print.h>
#include <stdio.h>
#include <platform.h> // for ports
#include <assert.h>
#include <stdlib.h> // exit status
#include <xccompat.h>
#include <stdint.h>
#include <print.h>

#include <string.h>
 

// ********** input with measurement do you want to perform
// T1 == time of a frame (without blank)
// T2 == time doing operations in a line
// T3 == time between frames
// T4 == time between lines 
#define MEASURE_T4 
#define DO_MEMCPY 0
#define TICKS_DELAY 1
// ***********

// I2C
#include "i2c.h"

// MIPI
#include "mipi_timing.h"

// Sensor
#define MSG_SUCCESS "Stream start OK\n"
#define MSG_FAIL "Stream start Failed\n"

// timing information
#define TIME_MSG_1 "elapsed\t%.6f\t[ms]\t%d\t[ticks]\n"
#define PRINT_TIME(y) printf("%d\n", y)
#define MS_MULTIPLIER 0.00001 

// Utils
#include "utils.h"

// Globals
#define FINAL_IMAGE_FILENAME "out.raw"
uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES];
char end_transmission = 0;
char found = 0;
mipi_data_type_t p_data_type = 0;
int t1, t2, t3, t4, t5, t6;

/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A; // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;

/**
 * The packet buffer is where the packet decoupler will tell the MIPI receiver
 * thread to store received packets.
 */
#define DEMUX_DATATYPE    0     // RESERVED
#define DEMUX_MODE        0x00  // no demux
#define DEMUX_EN          0     // DISABLE DEMUX
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER
#define REV(n) ((n << 24) | (((n>>16)<<24)>>16) |  (((n<<16)>>24)<<16) | (n>>24))


// Saves the image to a file. This is a bit tricky because we don't want to use an endless loop
void save_image_to_file(chanend flag)
{
  select
  {
    case flag :> int i:
      {
      // write to a file
      write_image(FINAL_IMAGE_FILENAME, &FINAL_IMAGE[0][0], MIPI_IMAGE_HEIGHT_PIXELS, MIPI_LINE_WIDTH_BYTES);
      delay_microseconds(200); // for stability
      exit(1); // end the program here
      break;
      }
  }
}

//TODO this should be in utils
int measure_time(){
  int y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}

unsafe {
static
void handle_packet(
    image_rx_t* img_rx,
    const mipi_packet_t* unsafe pkt,
    chanend flag)
  {
    
    // definitions
    const mipi_header_t header = pkt->header;
    const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);
    const unsigned is_long = MIPI_IS_LONG_PACKET(header);
    const unsigned word_count = MIPI_GET_WORD_COUNT(header);
    static uint8_t wait_for_clean_frame = 1; // static because it will change in the future
    //[debug] printf("packet header = 0x%08x, wc=%d \n", REV(header), word_count);


    // We return until the start of frame is reached
    if (wait_for_clean_frame == 1){
      if (data_type != MIPI_DT_FRAME_START){
        return;
      }
      else{
        wait_for_clean_frame = 0;
      }
    }
    // Use case by data type
    switch (data_type)
    {
        case MIPI_DT_FRAME_START: { // Start of frame. Just reset line number.
          #ifdef MEASURE_T3
            PRINT_TIME((measure_time() - t3));
            t3 = measure_time();
          #endif

          #ifdef MEASURE_T1
            t1 = measure_time();
          #endif

          img_rx->frame_number++;
          img_rx->line_number = 0;
          break;
        }

        case EXPECTED_FORMAT:{ // save it in SRAM and increment line

          #ifdef MEASURE_T4
            PRINT_TIME((measure_time() - t4));
            t4 = measure_time();
          #endif

          // if line number is grater than expected, just reset the line number
          if (img_rx->line_number >= MIPI_IMAGE_HEIGHT_PIXELS)
          {
            break; // let pass the rest until next frame
          }

          #if DO_MEMCPY
            // then copy
            not_silly_memcpy(
                &FINAL_IMAGE[img_rx->line_number][0],
                &pkt->payload[0],
                MIPI_LINE_WIDTH_BYTES); // here is data width
          #endif

          // burn some cycles
          delay_ticks(TICKS_DELAY);
          
          // printf("0x%04x,",pkt->payload[0]);

          // go for next line and exit
          img_rx->line_number++;
          
          break;
        }

        case MIPI_DT_EOT:{
          
          break;
        }

        case MIPI_DT_FRAME_END:{ // we signal that the frame is finish so we can write it to a file
          // printf("--------->> End\n");
          #ifdef MEASURE_T1
            PRINT_TIME((measure_time() - t1));
          #endif
          break;
        }

        default: // error with frame type or protected types
        {
          break;
        }
    }
  }


#pragma unsafe arrays
static
void mipi_packet_handler(
    streaming chanend c_pkt, 
    streaming chanend c_ctrl,
    chanend flag
    )
{
  image_rx_t img_rx = {0,0};  // stores the coordinates X, Y of the image
  unsigned pkt_idx = 0;       // packet index

  // Give the MIPI packet receiver a buffer
  outuint((chanend) c_pkt, (unsigned) &packet_buffer[pkt_idx]);
  pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);
  
  while(1) {

    #ifdef MEASURE_T6
      t6 = measure_time();
    #endif


    // Wait for the receiver thread to tell us a new packet was completed.
    mipi_packet_t * unsafe pkt = (mipi_packet_t*unsafe) inuint((chanend) c_pkt);


    // Give it a new buffer before processing the received one
    outuint((chanend) c_pkt, (unsigned) &packet_buffer[pkt_idx]);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);

    #ifdef MEASURE_T6
      PRINT_TIME((measure_time() - t6));
    #endif

    // Process the packet. We need to be finished with this and looped
    // back up to grab the next MIPI packet BEFORE the receiver thread
    // tries to give us the next packet.
    #ifdef MEASURE_T2
      t4 = measure_time();
    #endif

    handle_packet(&img_rx, pkt, flag);


    #ifdef MEASURE_T2
        PRINT_TIME((measure_time() - t4));
    #endif
  }
}
}


void mipi_main(client interface i2c_master_if i2c)
{
  printf("< Start of APP capture application >\n");
  streaming chan c_pkt;
  streaming chan c_ctrl;
  chan flag;
  

  // See AN for MIPI shim
  // 0x7E42 >> 0111 1110 0100 0010
  // in the explorer BOARD DPDN is swap
  write_node_config_reg(tile[MIPI_TILE],
                        XS1_SSWITCH_MIPI_DPHY_CFG3_NUM,
                        0x7E42); //TODO decompose into different values

  // send packet to MIPI shim
  MipiPacketRx_init(tile[MIPI_TILE],
                    p_mipi_rxd,
                    p_mipi_rxv,
                    p_mipi_rxa,
                    p_mipi_clk,
                    clk_mipi,
                    DEMUX_EN,
                    DEMUX_DATATYPE,
                    DEMUX_MODE,
                    MIPI_CLK_DIV,
                    MIPI_CFG_CLK_DIV);

  // Start camera and its configurations
  int r = 0;
  r |= camera_init(i2c);
  delay_milliseconds(100); //TODO include this inside the function
  r |= camera_configure(i2c);
  delay_milliseconds(500);
  
  // Start streaming mode
  r |= camera_start(i2c);
  delay_milliseconds(2000);

  if (r != 0){
    printf(MSG_FAIL);
  }
  else{
    printf(MSG_SUCCESS);
  }

  // ask the user to press the key "c" to capture the frame
  // user_input(); //TODO not working with a par job 


  // start the different jobs (packet controller, handler, and post_process)
  par
  {
    MipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, flag);
    save_image_to_file(flag);
  }
}

