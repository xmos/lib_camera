// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"

C_API_START

typedef struct {
    unsigned height;
    unsigned width;
    unsigned channels;
    int8_t *ptr;
} Image_t;

typedef struct
{
  // Offsets
  unsigned offset_x;
  unsigned offset_y;
  // Scale
  float sx;
  float sy;
  // Shear
  float shx;
  float shy;
  // Rotation
  float angle;
  // Transform
  float *T;
  // Delay
  unsigned delay;
  unsigned cmd;
} Camera_configure_t;


void camera_isp_thread(
  streaming_chanend_t c_pkt,
  streaming_chanend_t c_ctrl,
  chanend_t c_isp,
  chanend_t c_user);

void camera_isp_capture_in_ms(
  chanend_t c_user, 
  unsigned ms, 
  Image_t* image);

C_API_END
