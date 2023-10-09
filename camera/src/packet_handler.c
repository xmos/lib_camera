// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdbool.h>

#include <xcore/assert.h>
#include <xcore/channel.h>
#include <xcore/select.h>

#include "packet_handler.h"

#include "isp_pipeline.h"
#include "camera_api.h"
#include "camera_utils.h"
#include "sensor.h"

// Contains the local state info for the packet handler thread.
static frame_state ph_state = {
    1,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0   // out_line_number
};

static 
void handle_frame_start(chanend c_isp)
{
  // send to the ISP to reset the filters
  unsigned resp = isp_send_cmd(c_isp, FILTER_UPDATE);
  if (resp != RESP_OK){
    printf("Error in ISP\n");
  }
}


static
void handle_unknown_packet(
    const mipi_packet_t* pkt)
{
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);
  xassert(data_type < 0x3F && "Packet non valid"); // note [1]
}

static
unsigned handle_pixel_data(
    const mipi_packet_t* pkt,
    int8_t output_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS],
    chanend c_isp)
{

  // First, service any raw requests.
  camera_new_row((int8_t*) &pkt->payload[0], ph_state.in_line_number);

  

  //Send row info
  isp_send_cmd(c_isp, PROCESS_ROW);
  row_info_t row_info;
  row_info.row_ptr = (int8_t*) &pkt->payload[0];
  row_info.state_ptr = &ph_state;
  isp_send_row_info(c_isp, &row_info);

  // Bayer pattern is RGGB; even index rows have RG data, 
  // odd index rows have GB data.
  unsigned pattern = ph_state.in_line_number % 2;

  // Temporary buffer to store horizontally-filtered row data. [1]
  int8_t hfilt_row[APP_IMAGE_WIDTH_PIXELS];
 
  if(pattern == 0){ // Packet contains RGRGRGRGRGRGRGRGRG...
    ////// RED
    pixel_hfilter(&hfilt_row[0],
                  (int8_t*) &pkt->payload[0],
                  &hfilter_state[CHAN_RED].coef[0],
                  hfilter_state[CHAN_RED].acc_init,
                  hfilter_state[CHAN_RED].shift,
                  HFILTER_INPUT_STRIDE,
                  APP_IMAGE_WIDTH_PIXELS);

    
    image_vfilter_process_row(&output_buffer[CHAN_RED][0],
                              &vfilter_accs[CHAN_RED][0],
                              &hfilt_row[0]);

    ////// GREEN
    pixel_hfilter(&hfilt_row[0],
                  (int8_t*) &pkt->payload[0],
                  &hfilter_state[CHAN_GREEN].coef[0],
                  hfilter_state[CHAN_GREEN].acc_init,
                  hfilter_state[CHAN_GREEN].shift,
                  HFILTER_INPUT_STRIDE,
                  APP_IMAGE_WIDTH_PIXELS);

    // we now it is not the las row [2]
    image_vfilter_process_row(&output_buffer[CHAN_GREEN][0],
                              &vfilter_accs[CHAN_GREEN][0],
                              &hfilt_row[0]);

  } 
  else { // Packet contains GBGBGBGBGBGBGBGBGBGB...
    ////// BLUE
    pixel_hfilter(&hfilt_row[0],
                  (int8_t*) &pkt->payload[0],
                  &hfilter_state[CHAN_BLUE].coef[0],
                  hfilter_state[CHAN_BLUE].acc_init,
                  hfilter_state[CHAN_BLUE].shift,
                  HFILTER_INPUT_STRIDE,
                  APP_IMAGE_WIDTH_PIXELS);

    unsigned new_row = image_vfilter_process_row(
                            &output_buffer[CHAN_BLUE][0],
                            &vfilter_accs[CHAN_BLUE][0],
                            &hfilt_row[0]);

    // If new_row is true, then the vertical decimator has output a new row for
    // each of the three color channels, and so we should signal this upwards.
    if(new_row){
      camera_new_row_decimated(output_buffer, ph_state.out_line_number);
      ph_state.out_line_number++;
      return 1;
    }
  }
  return 0;
}


static //TODO this should be ISP
void handle_frame_end(
    int8_t pix_out[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS])
{
  // Drain the vertical filter's accumulators
  image_vfilter_drain(&pix_out[CHAN_RED][0], &vfilter_accs[CHAN_RED][0]);
  image_vfilter_drain(&pix_out[CHAN_GREEN][0], &vfilter_accs[CHAN_GREEN][0]);
  if(image_vfilter_drain(&pix_out[CHAN_BLUE][0], &vfilter_accs[CHAN_BLUE][0])){
    // Pass final row(s) to the statistics thread
    camera_new_row_decimated(pix_out, ph_state.out_line_number);
    ph_state.out_line_number++;
  }

  // Signal statistics thread to do frame-end work by sending NULL.
  // s_chan_out_word(c_stats, (unsigned) NULL);
}

void handle_no_expected_lines()
{
  if(ph_state.in_line_number >= SENSOR_RAW_IMAGE_HEIGHT_PIXELS){
    // We've received more lines of image data than we expected.
#ifdef ASSERT_ON_TOO_MANY_LINES
      xassert(0 && "Recieved too many lines");
#endif
  }
}

/**
 * Process a single packet.
 * 
 * This function keeps track of where we are within the input and output image
 * frames. It also passes the packet along to a function for processing
 * depending upon the data type.
 */
static
void handle_packet(
    const mipi_packet_t* pkt,
    chanend c_isp)
{
  /*
   * These buffers store rows of the fully decimated image. They are passed
   * along to the statistics thread once the packet handler thread no longer
   * needs them.
   *
   * Two are needed -- the one the decimator is currently filling, and the one
   * that the statistics thread is currently using.
   */
  __attribute__((aligned(8)))
  static int8_t output_buff[2][APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
  static bool out_dex = 0;


  // definitions
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);

  // At start-up we usually want to wait for a new frame before processing
  // anything
  if(ph_state.wait_for_frame_start 
     && data_type != MIPI_DT_FRAME_START) return;

  /*
    The idea here is that logic that keeps the packet handler in a coherent
    state, like tracking frame and line numbers, should go directly in here, but
    logic that actually interprets, processes or reacts to packet data should go
    into the individual functions.
  */
  switch(data_type)
  {
    case MIPI_DT_FRAME_START:
      ph_state.wait_for_frame_start = 0;
      ph_state.in_line_number = 0;
      ph_state.out_line_number = 0;
      ph_state.frame_number++;

      handle_frame_start(c_isp);   
      break;

    case MIPI_DT_FRAME_END:   
      handle_frame_end(output_buff[out_dex]);
      out_dex ^= 1;
      break;

    case MIPI_EXPECTED_FORMAT:     
      handle_no_expected_lines();

      unsigned new_row = handle_pixel_data(pkt, output_buff[out_dex], c_isp);
      if(new_row){
        out_dex ^= 1;
      }

      ph_state.in_line_number++;
      break;

    default:              
        // We've received a packet we don't know how to interpret.
      handle_unknown_packet(pkt);   
      break;
  }
}


void mipi_packet_handler(
    streaming_chanend_t c_pkt, 
    streaming_chanend_t c_ctrl,
    chanend c_isp)
{
  /*
   * These buffers will be used to hold received MIPI packets while they're
   * being processed.
   */
  __attribute__((aligned(8)))
  mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
  unsigned pkt_idx = 0;

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned) &packet_buffer[pkt_idx] );

  while(1) {
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT-1);

    // Swap buffers with the receiver thread. Give it the next buffer
    // to fill and take the last filled buffer from it.    
    mipi_packet_t * pkt = (mipi_packet_t*) s_chan_in_word(c_pkt);
    
    // Check is we are supose to stop or continue
    unsigned stop = camera_check_stop();
    
    if (stop == 1){
        // send stop to MipiReciever
        s_chan_out_word(c_pkt, (unsigned) NULL);
        puts("\nMipiPacketHandler: stop\n");
        return;
    }
    else{
        // send info to MipiReciever
        s_chan_out_word(c_pkt, (unsigned) &packet_buffer[pkt_idx]);
    }
    // Get information regarding the packet
    // const mipi_header_t header = pkt->header;
    // const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);

    // Process the packet 
    // unsigned time_start = measure_time();
    handle_packet(pkt, c_isp);
    // unsigned time_proc = measure_time() - time_start;
  }
}

/* Notes
[1]
uknown packets could be the following:
a - sensor specific packets (we let continue the app, uncomment print here for debug)
printf("Unknown packet type: %d\n", data_type);
b - invalid packets 

*/
