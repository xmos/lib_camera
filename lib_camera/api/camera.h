// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"    // chanend_t safe use from XC
#include "sensor_wrapper.h"  // sensor configuration

// High-Level Sensor Configuration
#define SENSOR_WIDHT                800
#define SENSOR_HEIGHT               800

#define CONFIG_FLIP                 FLIP_NONE
#define CONFIG_BINNING              BINNING_ON
#define CONFIG_CENTRALISE           CENTRALISE_ON

#ifndef CONFIG_APPLY_AWB
#define CONFIG_APPLY_AWB            (1)
#endif

#ifndef CONFIG_APPLY_AE
#define CONFIG_APPLY_AE             (1)
#endif


C_API_START

/**
 * @brief Lib camera main function
 * @param c_user  Channel to communicate with the user
 */
void camera_main(chanend_t c_camera);

C_API_END
