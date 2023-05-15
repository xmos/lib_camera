#include <xs1.h>
#include <print.h>
#include <stdio.h>
#include <platform.h> // for ports
#include <assert.h>
#include <stdlib.h> // exit status and malloc
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

// Globals
char end_transmission = 0;

typedef chanend chanend_t;


/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A; // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;


extern "C"
{
    void process_image(uint8_t *image, chanend_t c);
}


void control_exposure(chanend_t ch, client interface i2c_master_if i2c){
  while(1){
    select {
      case ch :> unsigned exposure:
      //printf("Exposure :> %d\n", exposure);
      imx219_set_gain_dB(i2c, exposure);
      break ;
    }
  }
}




unsafe {

static
void handle_packet(
    image_rx_t* img_rx,
    const mipi_packet_t* unsafe pkt,
    chanend flag,
    uint8_t* unsafe img_raw_ptr,
    chanend_t ch_exp)
  {
    // End here if just one transmission
    //if (end_transmission == 1){
    //  return;
    //}
    
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
    //printf("-- Im here 4--\n");
    // printf("dat type: 0x%08x\n",data_type);
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
          // then copy
          uint32_t pos = (img_rx->line_number) * MIPI_LINE_WIDTH_BYTES;
          // *** horizontal / vertical filtering ****


          // *******************************
          c_memcpy(&img_raw_ptr[pos], &pkt->payload[0], MIPI_LINE_WIDTH_BYTES); 
          // increase line number
          img_rx->line_number++;
          break;
        }

        case MIPI_DT_FRAME_END:{ // we signal that the frame is finish so we can write it to a file
          //if (end_transmission == 0){ //TODO not needed if and the end of transmission I just return
            
            //flag <: 1; 
            // wait_for_clean_frame = 1; // in case
            // end_transmission = 1;            
          //} 
          process_image(img_raw_ptr, ch_exp);
          //printf("-- Im here 3--\n");
          break;
        }

        default:{ // error with frame type or protected types or blank fields
          break;
        }
    }
  }

#pragma unsafe arrays
static void mipi_packet_handler(
    streaming chanend c_pkt,
    streaming chanend c_ctrl,
    chanend flag,
    uint8_t * unsafe image_ptr,
    chanend_t ch_exp)
{
  image_rx_t img_rx = {0, 0}; // stores the coordinates X, Y of the image
  unsigned pkt_idx = 0;       // packet index

  // Give the MIPI packet receiver a buffer
  outuint((chanend)c_pkt, (unsigned)&packet_buffer[pkt_idx]);
  pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);

  while (1)
  {
        // Wait for the receiver thread to tell us a new packet was completed.
        mipi_packet_t *unsafe pkt = (mipi_packet_t * unsafe) inuint((chanend)c_pkt);

        // Give it a new buffer before processing the received one
        outuint((chanend)c_pkt, (unsigned)&packet_buffer[pkt_idx]);
        pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);

        // Process the packet. We need to be finished with this and looped
        // back up to grab the next MIPI packet BEFORE the receiver thread
        // tries to give us the next packet.
        handle_packet(&img_rx, pkt, flag, image_ptr, ch_exp);

        //if (end_transmission == 1)
        //{
        //  return;
        //}
  }
}

} // unsafe region


void mipi_main(client interface i2c_master_if i2c)
{
  printf("< Start of APP capture application >\n");
  streaming chan c_pkt;
  streaming chan c_ctrl;
  chan flag;
  chan ch_exp;
  
  // allocate espace for the image buffer
  uint8_t* unsafe img_raw_ptr = malloc(MIPI_IMAGE_HEIGHT_PIXELS * MIPI_IMAGE_WIDTH_PIXELS * sizeof(uint8_t));
  
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
  r |= camera_start(i2c);
  delay_milliseconds(2000);

  if (r != 0){
    printf(MSG_FAIL);
  }
  else{
    printf(MSG_SUCCESS);
  }

  // start the different jobs (packet controller, handler, and post_process)
  par
  {
    gMipiPacketRx(p_mipi_rxd, p_mipi_rxa, c_pkt, c_ctrl);
    mipi_packet_handler(c_pkt, c_ctrl, flag, img_raw_ptr, ch_exp);
    //save_image_to_file(flag, img_raw_ptr);
    control_exposure(ch_exp, i2c);
  }
}


/*
  //uint16_t val = imx219_read(i2c, 0x0174);
  //printf("read value = %d\n", val);

  uint16_t gain_values[5];
  imx219_read_gains(i2c, gain_values);
  for (int i = 0; i < 5; i++)
  {
    printf("gain value = %d\n", gain_values[i]);
  }
*/