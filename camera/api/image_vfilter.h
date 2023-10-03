// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "sensor.h"

// The number of non-zero taps in the vertical filter.
#define VFILTER_TAP_COUNT   (5)

// The effective decimation factor of the vertical filter.
// Note that because of Bayering, this is half the horizontal decimation factor.
#define VFILTER_DEC_FACTOR  (APP_DECIMATION_FACTOR / 2)

// Required: Low-res image width must be multiple of 16 pixels
#if (((APP_IMAGE_WIDTH_PIXELS) >> 4) << 4) != (APP_IMAGE_WIDTH_PIXELS)
# error APP_IMAGE_WIDTH_PIXELS (width of decimated image) must be multiple of 16 pixels.
#endif

// The number of accumulators required for vertical filtering.
// ceil(tap_count / dec_factor)
#define VFILTER_ACC_COUNT  ((VFILTER_TAP_COUNT + VFILTER_DEC_FACTOR - 1)\
                              / (VFILTER_DEC_FACTOR))

// The value that the next_tap gets reset to after outputting a row.
#define VFILTER_RESET_INDEX   ((VFILTER_TAP_COUNT) - \
                  ( (VFILTER_DEC_FACTOR) * (VFILTER_ACC_COUNT) ))

#define VFILTER_ACC_WIDTH_SHORTS  (2 * APP_IMAGE_WIDTH_PIXELS)

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

typedef struct {
  int next_tap;
  int16_t buff[VFILTER_ACC_WIDTH_SHORTS];
} vfilter_acc_t;

/**
 * Initialize a vector of 32-bit split accumulators to a given value.
 *
 * This function is used to initialize a set of accumulators to an offset value
 * to prepare for filtering.
 *
 * The `pixel_vfilter_*()` functions use the VPU in 8-bit mode. All accumulators
 * are initialized to the same 32-bit value, with the values split as required
 * by the VPU.
 *
 * Accordingly, `pix_count` must be a multiple of 16.
 *
 * @param accs      The vector of accumulators to initialize.
 * @param acc_value The value to initialize the accumulators to.
 * @param pix_count The number of accumulators to initialize.
 */
void pixel_vfilter_acc_init(
    int16_t *accs,
    const int32_t acc_value,
    const unsigned pix_count);

/**
 * Generate output pixel values from a vector of accumulators.
 *
 * This function is to be called on a row of accumulators after the final filter
 * tap has been applied. The 32-bit accumulators are right-shifted by the values
 * in `shifts[]` (with rounding), saturated to 8-bit symmetric bounds, and
 * dropped from int32_t to int8_t before being written to `pix_out[]`.
 *
 * `pix_out[]`, `accs` and `shifts` must each be aligned to a 4-byte boundary.
 *
 * `accs` is expected to be formatted according to the VPU's split 32-bit
 * accumulator format.
 *
 * `shifts[k]` will be applied to accumulators with indices `k mod 16`. Usually,
 * each `shift[k]` will be the same.
 *
 * Due to VPU limitations, `pix_count` must be a multiple of 16.
 *
 * `pix_count` bytes will be written to `pix_out[]`.
 *
 * @param pix_out   The output pixel values.
 * @param accs      The vector of accumulators to generate output from.
 * @param shifts    The right-shifts to apply to each accumulator.
 * @param pix_count The number of output pixels.
 */
void pixel_vfilter_complete(
    int8_t *pix_out,
    const int16_t *accs,
    const int16_t shifts[16],
    const unsigned pix_count);

/**
 * Apply a filter tap to a vector of accumulators using the supplied input
 * pixels.
 *
 * This function is used to apply a single filter tap to a vector of
 * accumulators (i.e. `pix_count` separate filters are updated).
 *
 * If `ACC32[k]` is the 32-bit accumulator associated with output pixel `k`,
 * then this function computes:
 *
 *    ACC32[k] += (pix_in[k] * filter[k % 16])
 *
 * Because this function is used for incrementally applying the same vertical
 * filter across many columns of an image, the values `filter[k]` will typically
 * all be the same value, `COEF` for a given call to this function, simplifying
 * to:
 *
 *    ACC32[k] += (pix_in[k] * COEF)
 *
 * The `pix_in[]` array is expected to contain `pix_count` 8-bit pixels.
 *
 * `accs` is expected to be formatted according to the VPU's split 32-bit
 * accumulator format.
 *
 * `accs`, `pix_in` and `filter[]` must each be 4-byte aligned.
 *
 * Due to VPU limitations, `pix_count` must be a multiple of 16.
 *
 * When the final filter tap is applied, `pixel_vfilter_complete()` should be
 * called to generate output pixels from the accumulators.
 *
 * @param accs      The vector of accumulators to apply the filter tap to.
 * @param pix_in    The input pixels to apply to the filter.
 * @param filter    Vector containing filter coefficients.
 * @param pix_count The number of pixels to apply the filter to.
 */
void pixel_vfilter_macc(
    int16_t *accs,
    const int8_t *pix_in,
    const int8_t filter[16],
    const unsigned pix_count);

/**
 * @brief Initialize a vector of vertical filter accumulators.
 * 
 * Call this once at the start of each frame. The accumulator next_tap values
 * are set somewhat differently than image_vfilter_reset(), because the behavior
 * at the start of the image is a little different.
 * 
 * `acc_count` is the number of ROWS of accumulators, whereas
 * `image_width_lores` is the number of pixels per low-resolution image row 
 * (which is the number of individual accumulators PER ROW).
 * 
 * `image_width_lores` must be a multiple of 16 (atm)
 * 
 * @note vfilter functions are channel agnostic, so if the image is
 * separated into different color planes, this will need to be called once
 * for each color plane. If they're interleaved, it can be called once,
 * but the image width must then be the width in _bytes_. And it must
 * still be a multiple of 16.
 * 
 * @param accs The vector of accumulators to initialize.
 */
void image_vfilter_frame_init(
    vfilter_acc_t accs[]);

/**
 * @brief Apply a filter tap to a vector of accumulators using the supplied input
 *
 * @param output output vector after the filtering process
 * @param acc    vector of accumulators
 * @param pixel_data input data
 * @return unsigned 1 if rows are finished
 */
unsigned image_vfilter_process_row(
    int8_t output[],
    vfilter_acc_t acc[],
    const int8_t pixel_data[]);

/**
 * After the last line of the image, some of the accumulators will be midway
 * through processing the image but still need to be output without maccing
 * any more inputs.
 *
 * Keep calling this until it returns 0.
 *
 * @param output output vector after the filtering process
 * @param acc array of accumulators
 * @return unsigned 0 when finished
 */
unsigned image_vfilter_drain(
    int8_t output[],
    vfilter_acc_t acc[]);

#if defined(__XC__) || defined(__cplusplus)
}
#endif
