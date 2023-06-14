#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "sensor.h"

#define PRINT_TIME(a,b) printf("%d\n", b - a);

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif


/**
 * Measure the cpu ticks
 * 
 * @return ticks - Number of ticks
 */
unsigned measure_time();



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

#if defined(__XC__) || defined(__cplusplus)
}
#endif
