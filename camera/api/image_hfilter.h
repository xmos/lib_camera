// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

// The FIR filter used for horizontal filtering has 3 taps, and is
//      (0.20872991, 0.58254019, 0.20872991)
// Because of symmetry we can use a single coefficient for the even taps.
// The filter coefficients are scaled by a gain derived from the white balance
// algorithm, and then scaled to an appropriate fixed-point format.
#define COEF_B0   (0.58254019f)
#define COEF_B1   (0.20872991f)

/**
 * This structure holds the state for the horizontal filter.
 */
typedef struct {
  /// @brief  The initial value for the accumulator
  int32_t acc_init;
  /// @brief The filter coefficients
  int8_t coef[32];
  /// @brief The shift applied to the accumulator to get the output
  unsigned shift;
} hfilter_state_t;

/**
 * This function performs a horizontal filtering operation on a single row 
 * of pixels.
 *
 * The input and output arrays are assumed to be aligned to word boundaries.
 * 
 * The output pixels will be adjacent in memory, so this function is used to 
 * split an image with interleaved color planes into separate color planes.
 * 
 * @param output        The output array of pixels
 * @param input         The input array of pixels
 * @param coef          The filter coefficients
 * @param acc_init      The initial value for the accumulator
 * @param shift         The shift applied to the accumulator to get the output
 * @param input_stride  The number of input pixels to skip between output pixels
 * @param output_count  The number of output pixels to generate
 */
void pixel_hfilter(
    int8_t output[],
    const int8_t input[],
    const int8_t coef[32],
    const int32_t acc_init,
    const unsigned shift,
    const int32_t input_stride,
    const unsigned output_count);

/**
 * This function is used to update the filter parameters based on a gain to be
 * applied to the color channel represented by `state`.
 * 
 * `offset` is the byte offset (from the beginning of a row of pixels) of the
 * first pixel in the color channel represented by `state`. So, for a Bayered
 * RGGB image, the offset for the red channel would be 0, and the offset for
 * the blue channel would be 1. The offset for the green channel depends on 
 * whether the red-adjascent or blue-adjascent green pixel is used.
 * 
 * @param state   The filter state to update
 * @param gain    The gain to apply to the filter coefficients
 * @param offset  The offset into the filter coefficient array to start at.
 */
void pixel_hfilter_update_scale(
    hfilter_state_t* state,
    const float gain,
    const unsigned offset);
