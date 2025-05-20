// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>

#include <xscope.h>

// debug options
// (can be enabled via: -DDEBUG_PRINT_ENABLE_CAM_ISP=1)
#define DEBUG_UNIT CAM_ISP 
#include <debug_print.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_mipi.h"
#include "sensor_wrapper.h"


// -------- Globals & Constants -------------
static struct {
  unsigned wait_for_frame_start;
  unsigned frame_number;
  unsigned in_line_number;
  unsigned out_line_number;
  unsigned capture_finished;
  unsigned ae_value;
} ph_state = {
    0,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0,  // out_line_number
    0,  // capture_finished
    1,  // ae_value
};

// -------- State handlers --------
void camera_isp_start_capture_xscope(chanend_t c_cam, image_cfg_t *image) {
  xscope_int(CAP, 1);
  chan_out_buf_byte(c_cam, (uint8_t*)image, sizeof(image_cfg_t));
}
 
void camera_isp_get_capture_xscope(chanend_t c_cam) {
  xscope_int(CAP, 0);
  chan_in_byte(c_cam);
}

static
void handle_unknown_packet(
  mipi_data_type_t data_type) {
  xassert(data_type < 0x3F && "Packet non valid");
}

static
void handle_no_expected_lines() {
  if (ph_state.in_line_number >= SENSOR_HEIGHT) {
    // We've received more lines of image data than we expected.
    xassert(0 && "Recieved too many lines");
  }
}

static
void handle_post_process(image_cfg_t* image)
{

#if (CONFIG_APPLY_AWB)
  camera_isp_white_balance(image);
#endif

#if (CONFIG_APPLY_AE)
  ph_state.ae_value = camera_isp_auto_exposure(image);
  if (ph_state.ae_value) {
    camera_sensor_set_exposure(ph_state.ae_value);
  }
#endif
}


static
void handle_expected_lines(image_cfg_t* image, int8_t* data_in) {
  unsigned ln = ph_state.in_line_number;
  camera_mode_t mode = image->config->mode;
  
  // Check if the image region is valid
  uint8_t c1 = image->ptr == NULL;
  uint8_t c2 = ln < image->config->y1;
  uint8_t c3 = ln >= image->config->y2;
  if (c1 || c2 || c3) {
    return;
  }
  
  // Provide the image data to the user
  switch (mode)
  {
    case MODE_RAW:{
      camera_isp_raw8_to_raw8(image, data_in, ln);
      break;
    }
    case MODE_RGB1:{
      camera_isp_raw8_to_rgb1(image, data_in, ln);
      break;
    }
    case MODE_RGB2:{
      camera_isp_raw8_to_rgb2(image, data_in, ln);
      break;
    }
    case MODE_RGB4:{
      camera_isp_raw8_to_rgb4(image, data_in, ln);
      break;
    }
    default:{
      xassert(0 && "mode not supported");
      break;
    }
  }
}


// -------- Frame handling --------------

static
void camera_isp_packet_handler(
  const mipi_packet_t* pkt,
  image_cfg_t* image_cfg,
  chanend_t c_isp_to_user) {

  // Definitions
  const mipi_header_t header = pkt->header;
  const mipi_data_type_t data_type = MIPI_GET_DATA_TYPE(header);
  unsigned word_count = MIPI_GET_WORD_COUNT(header);
  xscope_int(WC, word_count);
  
  // Wait for a clean frame
  /*
  if (ph_state.wait_for_frame_start
    && data_type != MIPI_DT_FRAME_START) return;
  */

  // Data pointers calculation
  int8_t* data_in = (int8_t*)(&pkt->payload[0]);
  
  // Handle packets depending on their type
  switch (data_type) {
    case MIPI_DT_FRAME_START:
      xscope_int(SOF, ph_state.frame_number);
      ph_state.wait_for_frame_start = 0;
      ph_state.in_line_number = 0;
      ph_state.frame_number++;
      break;

    case MIPI_DT_RAW8:
      xscope_int(RAW8, ph_state.in_line_number);
      handle_no_expected_lines();
      handle_expected_lines(image_cfg, data_in);
      ph_state.in_line_number++;
      break;

    case MIPI_DT_FRAME_END:
      xscope_int(EOF, ph_state.frame_number - 1);
      camera_sensor_stop();
      if (image_cfg->ptr == NULL) {
        return;
      }
      handle_post_process(image_cfg);
      chan_out_byte(c_isp_to_user, 1);
      break;

    default:
      handle_unknown_packet(data_type);
      break;
  }
  return;
}


// -------- Main packet handler thread --------
void xscope_init_probes()
{
  // xscope init
  xscope_int(SOF, -1);
  xscope_int(EOF, -1);
  xscope_int(RAW8, -1);
  xscope_int(PCKT, -1);
  xscope_int(WC, -1);
}


void camera_isp_thread_xscope(
  streaming_chanend_t c_pkt,
  chanend_t c_ctrl,
  chanend_t c_cam)
{
  // Initialize xscope probes
  xscope_init_probes();

  // Initialize the MIPI packet receiver
  mipi_packet_t ALIGNED_8 packet_buffer[MIPI_PKT_BUFFER_COUNT];
  mipi_packet_t* pkt;
  unsigned pkt_idx = 0;

  // Image configuration
  image_cfg_t image;
  image.ptr = NULL;

  // Sensor configuration
  camera_sensor_init();
  //camera_sensor_start();

  // Wait for the sensor to start
  delay_milliseconds_cpp(500);

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);

  SELECT_RES(
    CASE_THEN(c_pkt, on_c_pkt_change),
    CASE_THEN(c_cam, on_c_user_isp_change)) {
  on_c_pkt_change: { // attending mipi_packet_rx
    pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);
    xscope_int(PCKT, pkt_idx);
    s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);
    camera_isp_packet_handler(pkt, &image, c_cam);
    continue;
    }
  on_c_user_isp_change: { // attending user_app
    // user petition
    chan_in_buf_byte(c_cam, (uint8_t*)&image, sizeof(image_cfg_t)); // recieve info from user
    // Start camera
    camera_sensor_start();
    continue;
    }
  }
}
