// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>

// xcore
#include <xccompat.h>

#include "sensor.h"

#define PRINT_I2C_REG 0
#define ENABLE_PRINT_SENSOR_CONTROL 0
#define ENCODE(cmd, arg) (((uint32_t)(cmd) << 16) | (uint32_t)(arg))
#define DECODE_CMD(value) ((uint16_t)((value) >> 16))
#define DECODE_ARG(value) ((uint16_t)((value) & 0xFFFF))

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

void sensor_control(chanend c_control);

#if defined(__XC__) || defined(__cplusplus)
}
#endif
