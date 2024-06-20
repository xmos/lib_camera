// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "api.h"
#include "xcore_compat.h"

#define N_CH_USER_ISP   2 // Number of channels between user and ISP
#define CH_USER_ISP     0 // Channel between user and ISP
#define CH_ISP_USER     1 // Channel between ISP and user

C_API_START

/**
 * @brief Lib camera main function
 * @param c_user  Channel to communicate with the user
 */
void lib_camera_main(chanend_t c_camera[N_CH_USER_ISP]);

C_API_END
