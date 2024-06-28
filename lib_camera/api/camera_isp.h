// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"
#include "camera.h" // packet size

#define MIPI_MAX_PKT_SIZE_BYTES     ((SENSOR_WIDHT) + 4)
#define MIPI_PKT_BUFFER_COUNT       (4)

C_API_START

// this struct will hold the configuration for the camera
typedef struct
{
  // Offsets: [0,1] range form the sensor
  float offset_x;
  float offset_y;
  // Scale: [1,4] scale
  float sx;        
  float sy;        
  // Shear: [0,1] shear ratio
  float shx;       
  float shy;      
  // Rotation: [-90,90] degrees
  float angle;
  // Transform: 3x3 matrix
  float* T;
  // Mipi region
  unsigned x1, y1, x2, y2;
} camera_configure_t;


// this will hold the image data and possible configurations
// we could split this into two structs, but for now we will keep it simple
// splitting will just cause have more functions, so is a balance to decide
typedef struct {
  unsigned height;
  unsigned width;
  unsigned channels;
  int8_t* ptr;
  camera_configure_t* config;
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
 * @brief This function will be the main thread for the ISP
 * 
 * @param c_pkt channel to receive mipi packets
 * @param c_ctrl channel to receive control messages from or to mipi
 * @param c_cam  channel array between user and isp
 */
void camera_isp_thread(
  streaming_chanend_t c_pkt,
  streaming_chanend_t c_ctrl,
  chanend_t c_cam[N_CH_USER_ISP]);


C_API_END
