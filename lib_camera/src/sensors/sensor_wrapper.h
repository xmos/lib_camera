// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>

#include <xccompat.h>

#include "api.h"

// I2C defines
#define I2C_DEV_ADDR             0x10
#define I2C_DEV_SPEED             400
#define PRINT_I2C_REG               0

// Sensor defines
typedef struct {
  uint16_t sensor_width;
  uint16_t sensor_height;
} resolution_t;

typedef enum {
  BINNING_OFF = 0,
  BINNING_ON = 1,
} binning_t;

typedef enum {
  CENTRALISE_OFF = 0,
  CENTRALISE_ON = 1,
} centralise_t;

typedef enum {
  FLIP_NONE = (0 | (0 << 1)),
  FLIP_VERTICAL = (0 | (1 << 1))
} orientation_t;

typedef enum{
  PATTERN_NONE = 0,
  PATTERN_SOLID_COLOR,
  PATTERN_COLOR_BARS,
  PATTERN_FADE_TO_GREY,
  PATTERN_PN_9,
  PATTERN_16_SPLIT,
  PATTERN_16_SPLIT_INV,
  PATTERN_COLUMN_COUNTER,
  PATTERN_INV_COL_COUNTER,
  PATTERN_PN_31,
} camera_patterns_t;

C_API_START

void camera_sensor_init();
void camera_sensor_start();
void camera_sensor_stop();
void camera_sensor_set_tp(camera_patterns_t pattern);

C_API_END
