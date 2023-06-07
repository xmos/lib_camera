
#pragma once

#include <stdint.h>

#include "sensor.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif


#define COEF_B0   (0.58254019f)
#define COEF_B1   (0.20872991f)


void pixel_hfilter(
    int8_t output[],
    const int8_t input[],
    const int8_t coef[32],
    const int32_t acc_init,
    const unsigned shift,
    const int32_t input_stride,
    const unsigned output_count);

typedef struct {
  int32_t acc_init;
  int8_t coef[32];
  unsigned shift;
} hfilter_state_t;


void image_hfilter_update_scale(
    hfilter_state_t* state,
    const float gain,
    const unsigned offset);

// astew: I'm confused about this function now...this is in the camera
//        code, but it's referencing macros that only make sense in the
//        context of an application, because applications should be able to
//        have different APP_IMAGE_WIDTH_PIXELS settings..
void image_hfilter(
    int8_t pix_out[APP_IMAGE_WIDTH_PIXELS],
    const hfilter_state_t* state,
    const int8_t pix_in[SENSOR_RAW_IMAGE_WIDTH_PIXELS]);
    

#if defined(__XC__) || defined(__cplusplus)
}
#endif
