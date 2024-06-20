// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <print.h>

#include "lib_camera.h"  // in reality image def could be in isp.h

#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_mipi_defines.h"
#include "xs3_memcpy.h"

#include "sensor_control.h"

#define ALIGNED_8 __attribute__((aligned(8)))


// -------- Globals --------
static frame_state_t ph_state = {
    1,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0   // out_line_number
};

sensor_control_t ctrl_stop = {
  .cmd = SENSOR_STREAM_STOP,
  .arg = 0
};

sensor_control_t ctrl_start = {
  .cmd = SENSOR_STREAM_START,
  .arg = 50
};

// -------- State handlers --------
static
void handle_unknown_packet(
  mipi_data_type_t data_type) {
  xassert(data_type < 0x3F && "Packet non valid");
}

static
void handle_no_expected_lines() {
  if (ph_state.in_line_number >= MIPI_IMAGE_HEIGHT_PIXELS) {
    // We've received more lines of image data than we expected.
    xassert(0 && "Recieved too many lines");
  }
}

static
void handle_end_of_frame(
  Image_cfg_t* image,
  chanend_t c_control,
  chanend_t c_isp_user)  
{
  printstrln("EOF");
  camera_isp_send_ctrl(c_control, &ctrl_stop);
  if (image->ptr != NULL) {
    chan_out_byte(c_isp_user, 1);
  }
}

// -------- ISP communication --------


// ISP <> USER
inline void camera_isp_send_cfg(
  chanend_t c_isp,
  Image_cfg_t* image) 
{
  chan_out_buf_byte(c_isp, (uint8_t*)image, sizeof(Image_cfg_t));
}

inline void camera_isp_recv_cfg(
  chanend_t c_isp,
  Image_cfg_t* image) 
{
  chan_in_buf_byte(c_isp, (uint8_t*)image, sizeof(Image_cfg_t));
}

// ISP <> CTRL
inline void camera_isp_send_ctrl(
  chanend_t c_ctrl,
  sensor_control_t* ctrl) 
{
  chan_out_buf_byte(c_ctrl, (uint8_t*)ctrl, sizeof(sensor_control_t));
}

inline void camera_isp_recv_ctrl(
  chanend_t c_ctrl,
  sensor_control_t* ctrl) 
{
  chan_in_buf_byte(c_ctrl, (uint8_t*)ctrl, sizeof(sensor_control_t));
}


// -------- Frame handling --------
static
void camera_isp_packet_handler(
  const mipi_packet_t* pkt,
  Image_cfg_t* image_cfg,
  chanend_t c_control,
  chanend_t c_isp_to_user) {
  
  // Definitions
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);

  // Wait for a clean frame
  if (ph_state.wait_for_frame_start
    && data_type != MIPI_DT_FRAME_START) return;

  // Timing
  static uint32_t t_init=0;
  static uint32_t t_end=0;

  // Data pointers calculation
  unsigned width = image_cfg->width;
  int8_t* data_in = (int8_t*)(&pkt->payload[0]);
  int8_t* data_out = &image_cfg->ptr[0] + (ph_state.in_line_number * width);

  // Handle packets depending on their type
  switch (data_type) {
    case MIPI_DT_FRAME_START:
      printstrln("SOF");
      t_init = get_reference_time();
      ph_state.wait_for_frame_start = 0;
      ph_state.in_line_number = 0;
      ph_state.out_line_number = 0;
      ph_state.frame_number++;
      break;

    case CONFIG_MIPI_FORMAT:
      handle_no_expected_lines();
      if (image_cfg->ptr != NULL && ph_state.in_line_number < image_cfg->height) { //TODO second condition inside another fn
        printuintln(ph_state.in_line_number); // can be removed
        xs3_memcpy(  //TODO inside another fn
          data_out,
          data_in,
          width);
      }
      ph_state.in_line_number++;
      break;

    case MIPI_DT_FRAME_END:
      t_end = get_reference_time();
      printf("\nFrame time: %lu cycles\n", t_end - t_init);
      handle_end_of_frame(image_cfg, c_control, c_isp_to_user);
      break;

    default:
      handle_unknown_packet(data_type);
      break;
  }
  return;
}


// -------- Main packet handler thread --------
void camera_isp_thread(
  streaming_chanend_t c_pkt,
  streaming_chanend_t c_ctrl,
  chanend_t c_control,
  chanend_t c_cam[N_CH_USER_ISP]) {

  __attribute__((aligned(8)))
  mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];
  mipi_packet_t* pkt;
  unsigned pkt_idx = 0;

  // channel unpack
  chanend_t c_user_isp = c_cam[CH_USER_ISP];
  chanend_t c_isp_user = c_cam[CH_ISP_USER];

  // Image configuration
  Image_cfg_t image;
  image.ptr = NULL;

  uint8_t is_EOF = 0;

  delay_milliseconds_cpp(2200); // Wait for the sensor to start

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);


  SELECT_RES(
    CASE_THEN(c_pkt, on_c_pkt_change),
    CASE_THEN(c_user_isp, on_c_user_isp_change)) {

  on_c_pkt_change: { // attending mipi_packet_rx
    pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);
    s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);
    camera_isp_packet_handler(pkt, &image, c_control, c_isp_user);
    continue;
    }
  on_c_user_isp_change: { // attending user_app
    // user petition
    camera_isp_recv_cfg(c_user_isp, &image); // so we can work with img data
    // Start camera
    camera_isp_send_ctrl(c_control, &ctrl_start);
    continue;
    }
  }

}
