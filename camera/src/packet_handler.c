// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>


#include <xcore/assert.h>
#include <xcore/select.h>

#include "packet_handler.h"

#include "isp_pipeline.h"
#include "camera_api.h"
#include "camera_utils.h"
#include "sensor.h"

// Contains the local state info for the packet handler thread.
static frame_state_t ph_state = {
    1,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0   // out_line_number
};
static row_info_t row_info;

// -------- Error handling --------
static
void handle_unknown_packet(
    mipi_data_type_t data_type)
{
  xassert(data_type < 0x3F && "Packet non valid");
}

static
void handle_no_expected_lines()
{
  if(ph_state.in_line_number >= SENSOR_RAW_IMAGE_HEIGHT_PIXELS){
    // We've received more lines of image data than we expected.
    #ifdef ASSERT_ON_TOO_MANY_LINES
          xassert(0 && "Recieved too many lines");
    #endif
  }
}

// -------- Frame handling --------
static 
void handle_frame_start(chanend_t c_isp)
{
  // send to the ISP to reset the filters
  isp_cmd_t resp = isp_send_cmd(c_isp, FILTER_UPDATE);
  xassert(resp == RESP_OK && "Error in ISP filter update\n");
}

static
void handle_pixel_data(
    const mipi_packet_t* pkt,
    chanend_t c_isp)
{
  // Send cmd to isp
  isp_send_cmd(c_isp, PROCESS_ROW);

  // Prepare row info and send it
  row_info.row_ptr = (int8_t*) &pkt->payload[0];
  row_info.state_ptr = &ph_state;
  isp_send_row_info(c_isp, &row_info); //TODO WAIT FOR RESPONSE

  // Wait for response
  isp_cmd_t resp = isp_wait(c_isp);
  xassert(resp == RESP_OK && "Error in ISP process row\n");
}

static 
void handle_frame_end(
    const mipi_packet_t* pkt,
    chanend_t c_isp)
{
  // Drain the vertical filter's accumulators
  // Send cmd to isp
  isp_send_cmd(c_isp, FILTER_DRAIN);

  // Prepare row info and send it
  row_info.row_ptr = (int8_t*) &pkt->payload[0];
  row_info.state_ptr = &ph_state;
  isp_send_row_info(c_isp, &row_info);

  //Handle frame end
  isp_cmd_t resp = isp_send_cmd(c_isp, PROCESS_EOF);
  xassert(resp == RESP_OK && "Error in ISP process EOF\n"); 
}


static
void handle_packet(
    const mipi_packet_t* pkt,
    chanend_t c_isp)
{
  // Definitions
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);

  // Wait for a clean frame
  if(ph_state.wait_for_frame_start 
     && data_type != MIPI_DT_FRAME_START) return;

  // Handle packets depending on their type
  switch(data_type)
  {
    case MIPI_DT_FRAME_START:
      ph_state.wait_for_frame_start = 0;
      ph_state.in_line_number = 0;
      ph_state.out_line_number = 0;
      ph_state.frame_number++;
      handle_frame_start(c_isp);   
      break;

    case MIPI_EXPECTED_FORMAT:     
      handle_no_expected_lines();
      handle_pixel_data(pkt, c_isp);
      ph_state.in_line_number++;
      break;

    case MIPI_DT_FRAME_END:   
      handle_frame_end(pkt, c_isp);
      break;

    default:              
      handle_unknown_packet(data_type);   
      break;
  }
}


// -------- Main packet handler thread --------
void mipi_packet_handler(
    streaming_chanend_t c_pkt, 
    streaming_chanend_t c_ctrl,
    chanend_t c_isp)
{

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
        puts("\n> MipiPacketHandler: stop\n");
        // send stop to ISP
        isp_send_cmd(c_isp, ISP_STOP);
        puts("> ISP: stop\n");

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
