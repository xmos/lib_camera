// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <print.h>

#include "camera.h"  // in reality image def could be in isp.h

#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_defs.h"
#include "sensor_wrapper.h"

#define ALIGNED_8 __attribute__((aligned(8)))


// -------- Globals -------------

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
  chanend_t c_isp_user)  
{
  printstrln("EOF");
  camera_sensor_stop();
  if (image->ptr != NULL) {
    chan_out_byte(c_isp_user, 1);
  }
}

static
void handle_expected_lines(Image_cfg_t* image, int8_t* data_in) {
  unsigned ln = ph_state.in_line_number;
  unsigned img_ln = ln - image->config->y1;

  // Check if the image region is valid
  uint8_t c1 = image->ptr == NULL;
  uint8_t c2 = ln < image->config->y1;
  uint8_t c3 = ln >= image->config->y2;
  if (c1 || c2 || c3) {
    return;
  }

  // we are in a valid region, copy the row
  // printuintln(img_ln);
  int8_t* data_src = data_in + image->config->x1;
  int8_t* data_dst = image->ptr + (img_ln * image->width);
  vpu_memcpy(
    data_dst,
    data_src,
    image->width);
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


// -------- Image transformation --------

inline
void camera_isp_coordinates_print(Image_cfg_t* img_cfg){
  camera_configure_t *cfg = img_cfg->config;
  printf("x1: %u, y1: %u, x2: %u, y2: %u\n", cfg->x1, cfg->y1, cfg->x2, cfg->y2);
}

void camera_isp_coordinates_compute(Image_cfg_t* img_cfg){
  camera_configure_t *cfg = img_cfg->config;

  // Compute the coordinates of the region of interest
  cfg->x1 = cfg->offset_x * SENSOR_WIDHT;
  cfg->y1 = cfg->offset_y * SENSOR_HEIGHT;
  cfg->x2 = cfg->x1 + img_cfg->width * cfg->sx;
  cfg->y2 = cfg->y1 + img_cfg->height * cfg->sy;

  // ensure all are even and unsigned
  cfg->x1 = ((unsigned)cfg->x1) & ~1;
  cfg->y1 = ((unsigned)cfg->y1) & ~1;
  cfg->x2 = ((unsigned)cfg->x2) & ~1;
  cfg->y2 = ((unsigned)cfg->y2) & ~1;

  // if flip vertical add one to y1 and y2
  if (CONFIG_FLIP == FLIP_VERTICAL) {
    cfg->y1 += 1;
    cfg->y2 += 1;
  }

  // ensure is logical
  xassert(cfg->x1 < cfg->x2 && "x1");
  xassert(cfg->y1 < cfg->y2 && "y1");
  xassert(cfg->x2 <= SENSOR_WIDHT && "x2");
  xassert(cfg->y2 <= SENSOR_HEIGHT && "y2");
}


// -------- Frame handling --------------

static
void camera_isp_packet_handler(
  const mipi_packet_t* pkt,
  Image_cfg_t* image_cfg,
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
  int8_t* data_in = (int8_t*)(&pkt->payload[0]);
  

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
      handle_expected_lines(image_cfg, data_in);
      ph_state.in_line_number++;
      break;

    case MIPI_DT_FRAME_END:
      t_end = get_reference_time();
      printf("\nFrame time: %lu cycles\n", t_end - t_init);
      handle_end_of_frame(image_cfg, c_isp_to_user);
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

  // Sensor configuration
  camera_sensor_init();

  // Wait for the sensor to start
  delay_milliseconds_cpp(1200); 

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);


  SELECT_RES(
    CASE_THEN(c_pkt, on_c_pkt_change),
    CASE_THEN(c_user_isp, on_c_user_isp_change)) {

  on_c_pkt_change: { // attending mipi_packet_rx
    pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);
    s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);
    camera_isp_packet_handler(pkt, &image, c_isp_user);
    continue;
    }
  on_c_user_isp_change: { // attending user_app
    // user petition
    camera_isp_recv_cfg(c_user_isp, &image); // so we can work with img data
    // Start camera
    camera_sensor_start();
    continue;
    }
  }

}
