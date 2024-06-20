// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include "api.h"

C_API_START

void delay_ticks_cpp(unsigned ticks);
void delay_milliseconds_cpp(unsigned delay);
void delay_seconds_cpp(unsigned int delay);

/**
 * Convert an array of int8 to an array of uint8.
 * 
 * Data can be updated in-place.
 * 
 * @param output - Array of uint8_t that will contain the output
 * @param input - Array of int8_t that contains the input
 * @param length - Length of the input and output arrays
 */
void vect_int8_to_uint8(
    uint8_t output[],
    int8_t input[], 
    const unsigned length);

C_API_END
