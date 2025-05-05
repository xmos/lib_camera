// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"    // chanend_t safe use from XC
#include "sensor_wrapper.h"  // sensor configuration

C_API_START

/** 
 * @addtogroup camera_main
 * @brief TBD
 * @{
 */

// High-Level Sensor Configuration
#define SENSOR_WIDHT    800 ///< Sensor width in pixels
#define SENSOR_HEIGHT   800 ///< Sensor height in pixels

#define CONFIG_FLIP         FLIP_NONE ///< Flip mode: FLIP_NONE, FLIP_VERTICAL
#define CONFIG_BINNING      BINNING_ON  ///< Binning mode: BINNING_ON or BINNING_OFF
#define CONFIG_CENTRALISE   CENTRALISE_ON ///< Centralise mode: CENTRALISE_ON or CENTRALISE_OFF

#ifndef CONFIG_APPLY_AWB
#define CONFIG_APPLY_AWB    (1) ///< Apply White Balance: 1 to apply, 0 to skip
#endif

#ifndef CONFIG_APPLY_AE
#define CONFIG_APPLY_AE     (1) ///< Apply Auto Exposure: 1 to apply, 0 to skip
#endif

/**
 * @brief Lib camera main function
 * @param c_user  Channel to communicate with the user
 * TODO: Add more details about the function and its parameters.
 */
void camera_main(chanend_t c_camera);

/// @} endgroup camera_main

C_API_END
