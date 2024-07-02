// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"

C_API_START

void delay_ticks_cpp(unsigned ticks);
void delay_milliseconds_cpp(unsigned delay);
void delay_seconds_cpp(unsigned int delay);

C_API_END
