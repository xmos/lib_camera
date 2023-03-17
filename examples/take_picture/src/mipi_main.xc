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
static mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
#define DEMUX_DATATYPE 0 // RESERVED
#define DEMUX_MODE 0x00  // no demux
#define DEMUX_EN 0
#define MIPI_CLK_DIV 1
#define MIPI_CFG_CLK_DIV 3
#define REV(n) ((n << 24) | (((n>>16)<<24)>>16) |  (((n<<16)>>24)<<16) | (n>>24))


// global vars
#define EXPECTED_FORMAT MIPI_DT_RAW10 //TODO should not be here
#define MIPI_LINE_WIDTH_bits MIPI_LINE_WIDTH_BYTES*sizeof(uint8_t)


void out_image(chanend flag)
{
  select
  {
    case flag :> int i:
      {
      // write to a file
      write_image();
      delay_microseconds(100);
      assert(0);
      break;
      }
  }
}

char wait = 0;
uint16_t j=0;
uint16_t i=0;
int count = 0;
char enable = 0;
int end_transmission = 0;

mipi_data_type_t p_data_type = 0;

// this are poissble data types that could be between SOF and DATA
int arr[] = {0x0000, 0x0012, 0x0007, 0x003e};
int len = sizeof(arr) / sizeof(arr[0]); // Calculate the size of the array

int findValue(mipi_data_type_t X) {
    for (int i = 0; i < len; i++) {
        if (arr[i] == X) {
            return 1;
        }
    }
    return 0; // X was not found
}

int found = 0;


unsafe {
static
void handle_packet(
    image_rx_t* img_rx,
    const mipi_packet_t* unsafe pkt,
    chanend flag)
  {
    if (end_transmission == 1){
      return;
    }
    const mipi_header_t header = pkt->header;
    const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);
    const unsigned is_long = MIPI_IS_LONG_PACKET(header);
    const unsigned word_count = MIPI_GET_WORD_COUNT(header);
    
    // printf("packet header = 0x%08x, wc=%d \n", REV(header), word_count);
    // here we wait for a clean start of frame after a payload
    if (found == 0){
      if (findValue(p_data_type) && data_type == EXPECTED_FORMAT) {
        printf("good to go\n");
        p_data_type = data_type;
        found = 1;
      }  
      else{
        p_data_type = data_type;
        if (found == 0){
          return;
        }
      }
    }

    switch (data_type)
    {
        case MIPI_DT_FRAME_START: // Start of frame. Just reset line number.
        {
          img_rx->frame_number++;
          img_rx->line_number = 0;
          break;
        }
        
        case EXPECTED_FORMAT: // save it in SRAM and increment line
        {
          not_silly_memcpy(
              &FINAL_IMAGE[img_rx->line_number][0], 
              &pkt->payload[0], 
              MIPI_LINE_WIDTH_BYTES);
          
          img_rx->line_number++;
          break;
        }

        case MIPI_DT_FRAME_END:
        {
          if (end_transmission == 0)
          {
            flag <: 1; 
            end_transmission = 1;
          } 
          break;
        }

        default: // error with frame type
        {
          // printchar('X');
          // printchar('\n'); 
          break;
        }
    }

    // printuint(img_rx->frame_number);
    // printchar('-');
    // printuintln( img_rx->line_number);
  }


#pragma unsafe arrays
static
void mipi_packet_handler(
    streaming chanend c_pkt, 
    streaming chanend c_ctrl,
    chanend flag
    )
{
  mipi_header_t mipiHeader;
  image_rx_t img_rx = {0,0};
  unsigned pkt_idx = 0;
  // unsigned in_frame = 0;
  
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
  }
}
}


void mipi_main(client interface i2c_master_if i2c)
{
  //printf("< Start of MIPI >\n");
  
  streaming chan c_pkt;
  streaming chan c_ctrl;
  chan flag;

  // See AN for MIPI shim
  // 0x7E42 >> 0111 1110 0100 0010
  // in the explorer BOARD DPDN is swap
  write_node_config_reg(tile[MIPI_TILE],
                        XS1_SSWITCH_MIPI_DPHY_CFG3_NUM,
                        0x7E42);

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

  // Now start the camera
  //TODO  replace with camera init
  int r = imx219_init(i2c);
  delay_milliseconds(1000);
  
  //TODO replace with camera start 
  r |= imx219_stream_start(i2c);
  delay_milliseconds(2000);

  if (r != 0)
  {
    printf(MSG_FAIL);
  }
  else
  {
    printf(MSG_SUCCESS);
  }

  par
  {
    MipiPacketRx2(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, flag);
    out_image(flag);
  }

  // return
  //printf("Return code = %d\n", r);
  //printf("< End of MIPI >\n");
}

