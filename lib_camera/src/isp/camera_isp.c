// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>
#include <xcore/select.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>

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
} ph_state = {
    1,  // wait_for_frame_start
    0,  // frame_number
    0,  // in_line_number
    0,   // out_line_number
    0   // capture_finished
};

const unsigned sensor_width_max_values[] = {
  MODE_RAW_MAX_SIZE,
  MODE_RGB1_MAX_SIZE,
  MODE_RGB2_MAX_SIZE,
  MODE_RGB4_MAX_SIZE
};

// -------- Image transformations --------


// -------- State handlers --------

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
void handle_end_of_frame(
  image_cfg_t* image,
  chanend_t c_cam)
{
  camera_sensor_stop();
  if (image->ptr != NULL) {
    ph_state.capture_finished = 1;
    chan_out_byte(c_cam, 1);
  }
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



// -------- Image Coordinates --------

inline
void camera_isp_coordinates_print(image_cfg_t* img_cfg){
  camera_cfg_t *cfg = img_cfg->config;
  printf("x1: %d, y1: %d, x2: %d, y2: %d\n", cfg->x1, cfg->y1, cfg->x2, cfg->y2);  
}

void camera_isp_coordinates_compute(image_cfg_t* img_cfg){
  camera_cfg_t *cfg = img_cfg->config;

  // If RAW, scale = 1
  unsigned scale = (cfg->mode == MODE_RAW) ? 1 : (unsigned)(cfg->mode);

  // Compute the coordinates of the region of interest
  cfg->x1 = cfg->offset_x * SENSOR_WIDHT;
  cfg->y1 = cfg->offset_y * SENSOR_HEIGHT;
  cfg->x2 = cfg->x1 + img_cfg->width * scale;
  cfg->y2 = cfg->y1 + img_cfg->height * scale;

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

  // compute sensor width and height
  cfg->sensor_width = cfg->x2 - cfg->x1;
  cfg->sensor_height = cfg->y2 - cfg->y1;
  unsigned mode = cfg->mode;
  unsigned max_size = sensor_width_max_values[mode];
  
  // if raw ensure channels are 1, else 3
  unsigned cond_raw = (mode == MODE_RAW && img_cfg->channels == 1);
  unsigned cond_rgb = (mode != MODE_RAW && img_cfg->channels == 3);
  
  // debug info
  debug_printf("Coords: x1:%d, y1:%d, x2:%d, y2:%d\n", cfg->x1, cfg->y1, cfg->x2, cfg->y2);
  debug_printf("Sensor: w:%d, h:%d\n", cfg->sensor_width, cfg->sensor_height);
  debug_printf("Mode: %d\n", mode);

  // ensure everything is logical
  xassert(cond_raw || cond_rgb && "channels not valid");
  xassert(cfg->sensor_width <= max_size && "sensor_width");
  xassert(cfg->sensor_height <= max_size && "sensor_height");
  xassert(cfg->x1 < cfg->x2 && "x1");
  xassert(cfg->y1 < cfg->y2 && "y1");
  xassert(cfg->x2 <= SENSOR_WIDHT && "x2");
  xassert(cfg->y2 <= SENSOR_HEIGHT && "y2");
}

// -------- Image API -------------------
inline 
void camera_isp_start_capture(chanend_t c_cam, image_cfg_t *image) {
  chan_out_buf_byte(c_cam, (uint8_t*)image, sizeof(image_cfg_t));
}

inline 
void camera_isp_get_capture(chanend_t c_cam) {
  chan_in_byte(c_cam);
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
      debug_printf("SOF\n");
      t_init = get_reference_time();
      ph_state.wait_for_frame_start = 0;
      ph_state.in_line_number = 0;
      //ph_state.out_line_number = 0;
      ph_state.capture_finished = 0;
      ph_state.frame_number++;
      break;

    case MIPI_DT_RAW8:
      handle_no_expected_lines();
      handle_expected_lines(image_cfg, data_in);
      ph_state.in_line_number++;
      break;

    case MIPI_DT_FRAME_END:
      debug_printf("EOF\n");
      t_end = get_reference_time();
      debug_printf("Frame time: %d cycles\n", t_end - t_init);
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
  chanend_t c_ctrl,
  chanend_t c_cam) {

  mipi_packet_t ALIGNED_8 packet_buffer[MIPI_PKT_BUFFER_COUNT];
  mipi_packet_t* pkt;
  unsigned pkt_idx = 0;

  // Image configuration
  image_cfg_t image;
  image.ptr = NULL;

  // Sensor configuration
  camera_sensor_init();

  // Wait for the sensor to start
  delay_milliseconds_cpp(1200);

  // Give the MIPI packet receiver a first buffer
  s_chan_out_word(c_pkt, (unsigned)&packet_buffer[pkt_idx]);


  SELECT_RES(
    CASE_THEN(c_pkt, on_c_pkt_change),
    CASE_THEN(c_cam, on_c_user_isp_change)) {
  on_c_pkt_change: { // attending mipi_packet_rx
    pkt = (mipi_packet_t*)s_chan_in_word(c_pkt);
    pkt_idx = (pkt_idx + 1) & (MIPI_PKT_BUFFER_COUNT - 1);
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
