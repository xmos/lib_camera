// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include <print.h>
#include <xcore/hwtimer.h>

#include "api.h"


C_API_START

/** 
 * @defgroup camera_utils Utility functions for camera operations
 * @{
 */

void delay_ticks_cpp(unsigned ticks);
void delay_milliseconds_cpp(unsigned delay);
void delay_seconds_cpp(unsigned int delay);
void xmemcpy(
  void* dst,
  const void* src,
  unsigned bytes);

/// @} endgroup camera_utils

C_API_END
