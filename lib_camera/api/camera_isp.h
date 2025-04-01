// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"
#include "camera.h" // packet size

// MIPI packet size
#define MIPI_MAX_PKT_SIZE_BYTES     ((SENSOR_WIDHT) + 4)
#define MIPI_PKT_BUFFER_COUNT       (4)

// Sensor width maximum values
#define MODE_RAW_MAX_SIZE           (800)
#define MODE_RGB1_MAX_SIZE          (200)
#define MODE_RGB2_MAX_SIZE          (400)
#define MODE_RGB4_MAX_SIZE          (800)


C_API_START

// this enum will hold the camera modes
typedef enum{
  MODE_RAW  = 0,
  MODE_RGB1 = 1,
  MODE_RGB2 = 2,
  MODE_RGB4 = 4
} camera_mode_t;

// this struct will hold the configuration for the camera
typedef struct
{
  float offset_x;           // [0,1] range form the sensor
  float offset_y;           // [0,1] range form the sensor
  camera_mode_t mode;       // RAW or RGB
  unsigned x1, y1, x2, y2;  // Mipi region
  unsigned sensor_width;    // Mipi region width
  unsigned sensor_height;   // Mipi region height
} camera_cfg_t;

// this will hold the image data and possible configurations
// we could split this into two structs, but for now we will keep it simple
// splitting will just cause have more functions, so is a balance to decide
typedef struct {
  unsigned height;
  unsigned width;
  unsigned channels;
  unsigned size;
  int8_t* ptr;
  camera_cfg_t* config;
} image_cfg_t;

// this struct will hold the mipi header and data
typedef unsigned mipi_header_t;
typedef struct {
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;


/**
 * @brief compute MIPI coordinates, from user request to sensor dimensions.
 *
 * @param image_cfg
 */
void camera_isp_coordinates_compute(image_cfg_t* image_cfg);

/**
 * @brief prints the coordinates of the image_cfg
 *
 * @param image_cfg
 */
void camera_isp_coordinates_print(image_cfg_t* image_cfg);


/**
 * @brief send camera configuration to isp and starts capture
 * 
 * @param c_cam camera channel
 * @param image image pointer and conficuration
 */
void camera_isp_start_capture(chanend_t c_cam, image_cfg_t *image);


/**
 * @brief recieves image from isp
 * 
 * @param c_cam camera channel
 * @param image image pointer and conficuration
 */
void camera_isp_get_capture(chanend_t c_cam);

/**
 * @brief This function will be the main thread for the ISP
 *
 * @param c_pkt channel to receive mipi packets
 * @param c_ctrl channel to receive control messages from or to mipi
 * @param c_cam  channel array between user and isp
 */
void camera_isp_thread(
  streaming_chanend_t c_pkt,
  chanend_t c_ctrl,
  chanend_t c_cam);


// -------- RAW to RGB -------------------
void camera_isp_raw8_to_raw8(image_cfg_t* image, int8_t* data_in, unsigned ln);
void camera_isp_raw8_to_rgb1(image_cfg_t* image, int8_t* data_in, unsigned ln);
void camera_isp_raw8_to_rgb2(image_cfg_t* image, int8_t* data_in, unsigned ln);
void camera_isp_raw8_to_rgb4(image_cfg_t* image, int8_t* data_in, unsigned ln);

// -------- White Balancing -------------------
void camera_isp_white_balance(image_cfg_t* image);

C_API_END
