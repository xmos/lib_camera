// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"    // chanend_t safe use from XC
#include "sensor_wrapper.h"  // sensor configuration

// Channels between user and ISP
#define N_CH_USER_ISP   2 // Number of channels between user and ISP
#define CH_USER_ISP     0 // Channel between user and ISP
#define CH_ISP_USER     1 // Channel between ISP and user

// High-Level Sensor Configuration
#define SENSOR_WIDHT                800
#define SENSOR_HEIGHT               800

#define CONFIG_FLIP                 FLIP_NONE
#define CONFIG_BINNING              BINNING_ON
#define CONFIG_CENTRALISE           CENTRALISE_ON

C_API_START

/**
 * @brief Lib camera main function
 * @param c_user  Channel to communicate with the user
 */
void camera_main(chanend_t c_camera);

C_API_END
