// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include "camera_mipi.h" // for mipi data type == sensor data type

typedef enum {
  FLIP_NONE = (0 | (0 << 1)),
  FLIP_VERTICAL = (0 | (1 << 1))
} orientation_t;

typedef enum {
  BINNING_OFF = 0,
  BINNING_ON = 1,
} binning_t;

typedef enum {
  CENTRALISE_OFF = 0,
  CENTRALISE_ON = 1,
} centralise_t;

typedef struct {
  uint16_t sensor_width;
  uint16_t sensor_height;
} resolution_t;

typedef mipi_data_type_t pixel_format_t;
