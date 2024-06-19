// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdint.h>

#include <xccompat.h>
#include <xcore/parallel.h>

#include "api.h"
#include "camera_mipi_defines.h"

// I2C defines
#define I2C_DEV_ADDR             0x10
#define I2C_DEV_SPEED             400
#define PRINT_I2C_REG               0

// Sensor defines
#define MODE_VGA_640x480         0x01       
#define MODE_1280x960            0x02

// Mipi defines
#define MIPI_IMAGE_WIDTH_BYTES          640
#define MIPI_IMAGE_HEIGHT_PIXELS        480

// Mipi Configuration
#define CONFIG_MODE                     MODE_VGA_640x480
#define CONFIG_MIPI_FORMAT              MIPI_DT_RAW8

// Legacy (to delete)
#define ENCODE(cmd, arg) (((uint32_t)(cmd) << 16) | (uint32_t)(arg & 0xFFFF))
#define DECODE_CMD(value) ((uint16_t)((value) >> 16))
#define DECODE_ARG(value) ((uint16_t)(value))

typedef mipi_data_type_t pixel_format_t;

typedef enum {
  SENSOR_INIT = 0,
  SENSOR_CONFIG,
  SENSOR_STREAM_START,
  SENSOR_STREAM_STOP,
  SENSOR_SET_EXPOSURE
} sensor_control_t;

typedef enum {
  RES_640_480 = MODE_VGA_640x480,
  RES_1280_960 = MODE_1280x960
} resolution_t;

C_API_START

/**
 * @brief Sensor control function
 * this is a C wrapper, it will just instantiate the sensor object
 * and call the control function
 * @param c_control Channel to control the sensor
 */
void sensor_control(chanend c_control);

C_API_END
