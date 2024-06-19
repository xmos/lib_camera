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

C_API_START

/**
 * @brief Sensor control function
 * this is a C wrapper, it will just instantiate the sensor object
 * and call the control function
 * @param c_control Channel to control the sensor
 */
void sensor_control(chanend c_control);

C_API_END
