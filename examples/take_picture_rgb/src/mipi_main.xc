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
 
// I2C
#include "i2c.h"

// MIPI
#include "mipi_main.h"
#include "MipiPacket.h"

// Sensor
#define MSG_SUCCESS "Stream start OK\n"
#define MSG_FAIL "Stream start Failed\n"

// Image 
#include "process_frame.h"

#define RED 0
#define GREEN 1
#define BLUE 2

// Globals
char end_transmission = 0;
mipi_data_type_t p_data_type = 0;


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
      // raw_to_rgb();
      // write_image();
      write_image_rgb();
      delay_microseconds(200); // for stability //TODO maybe inside the function
      exit(1); // end the program here
      break;
      }
  }
}

unsafe {
static
void handle_packet(
    image_rx_t* img_rx,
    const mipi_packet_t* unsafe pkt,
    chanend flag)
  {
    // End here if just one transmission
    if (end_transmission == 1){
      return;
    }

    // definitions
    const mipi_header_t header = pkt->header;
    const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);
    const unsigned is_long = MIPI_IS_LONG_PACKET(header);  // not used for the moment
    const unsigned word_count = MIPI_GET_WORD_COUNT(header); // not used for the moment
    static uint8_t wait_for_clean_frame = 1; // static because it will change in the future
    // printf("packet header = 0x%08x, wc=%d \n", REV(header), word_count);

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
          img_rx->frame_number++;
          img_rx->line_number = 0;
          break;
        }

        case EXPECTED_FORMAT:{ // save it in SRAM and increment line
          // if line number is grater than expected, just reset the line number
          if (img_rx->line_number >= MIPI_IMAGE_HEIGHT_PIXELS){
            break; // let pass the rest until next frame
          }
          // copy thepending of the row
          uint16_t newline = (img_rx->line_number >> 1);
          for (uint16_t i = 0; i < MIPI_LINE_WIDTH_BYTES - 2; i = i + 4){
            if ((img_rx->line_number % 2) == 0){ // even
              FINAL_IMAGE[newline][(i >> 1)][RED] = pkt->payload[i]; //RED
              FINAL_IMAGE[newline][(i >> 1)][GREEN] = pkt->payload[i+1]; //GREEN
            }
            else{
              // FINAL_IMAGE[newline][i][RED]; //GREEN 2
              // FINAL_IMAGE[newline][(i >> 1)][BLUE] = pkt->payload[i+1]; //BLUE
            }
          }
          
          /*
          // then copy
          not_silly_memcpy(
              &FINAL_IMAGE[img_rx->line_number][0],
              &pkt->payload[0],
              MIPI_LINE_WIDTH_BYTES); // here is data width
          */
          img_rx->line_number++;
          break;
        }

        case MIPI_DT_FRAME_END:{ // we signal that the frame is finish so we can write it to a file
          if (end_transmission == 0){ //TODO not needed if and the end of transmission I just return
            flag <: 1; 
            end_transmission = 1;            
          } 
          break;
        }

        default:{ // error with frame type or protected types
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
    // Wait for the receiver thread to tell us a new packet was completed.
    mipi_packet_t * unsafe pkt = (mipi_packet_t*unsafe) inuint((chanend) c_pkt);

    // Give it a new buffer before processing the received one
    outuint((chanend) c_pkt, (unsigned) &packet_buffer[pkt_idx]);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);

    // Process the packet. We need to be finished with this and looped
    // back up to grab the next MIPI packet BEFORE the receiver thread
    // tries to give us the next packet.
    handle_packet(&img_rx, pkt, flag);

    if (end_transmission == 1){
      return;
    }
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
    gMipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, flag);
    save_image_to_file(flag);
  }
}

