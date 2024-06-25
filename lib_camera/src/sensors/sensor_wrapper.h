// Copyright 2023-2024 XMOS LIMITED.
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

C_API_START

void camera_sensor_init();
void camera_sensor_start();
void camera_sensor_stop();

C_API_END
