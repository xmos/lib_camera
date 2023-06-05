
#pragma once

#include <stdint.h>

#include "sensor.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif


void pixel_hfilter(
    int8_t output[],
    const int8_t input[],
    const int8_t coef[32],
    const int16_t acc_hi[16],
    const int16_t acc_lo[16],
    const int16_t acc_shr[16],
    const int32_t input_stride,
    const unsigned output_count);

void image_hfilter(
    int8_t pix_out[APP_IMAGE_WIDTH_PIXELS],
    const int8_t pix_in[SENSOR_RAW_IMAGE_WIDTH_PIXELS],
    const unsigned channel_index);
    


extern
const int8_t hfilter_coef_bayered_even[32];

extern
int8_t hfilter_red[32];
extern
int8_t hfilter_green[32];
extern
int8_t hfilter_blue[32];

extern
const int8_t hfilter_coef_bayered_odd[32];

extern
const int16_t hfilter_acc_init[2][16];

extern
const int16_t hfilter_shift[16];


#if defined(__XC__) || defined(__cplusplus)
}
#endif
