// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>

// xcore
#include <xccompat.h>

#include "sensor.h"

typedef enum {
  SENSOR_INIT = 0,
  SENSOR_CONFIG,
  SENSOR_STREAM_START,
  SENSOR_STREAM_STOP,
  SENSOR_SET_EXPOSURE
} sensor_control_t;

#define I2C_DEV_ADDR 0x10
#define I2C_DEV_SPEED 400
#define PRINT_I2C_REG 0
#define ENABLE_PRINT_SENSOR_CONTROL 0
#define ENCODE(cmd, arg) (((uint32_t)(cmd) << 16) | (uint32_t)(arg & 0xFFFF))
#define DECODE_CMD(value) ((uint16_t)((value) >> 16))
#define DECODE_ARG(value) ((uint16_t)(value))

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

void sensor_control(chanend_t c_control);

#if defined(__XC__) || defined(__cplusplus)
}
#endif
