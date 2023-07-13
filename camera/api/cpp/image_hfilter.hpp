#pragma once

#include <cstdint>

#include "sensor.h"

#ifdef __cplusplus


// The FIR filter used for horizontal filtering has 3 taps, and is
//      (0.20872991, 0.58254019, 0.20872991)
// Because of symmetry we can use a single coefficient for the even taps.
// The filter coefficients are scaled by a gain derived from the white balance
// algorithm, and then scaled to an appropriate fixed-point format.
#define COEF_B0   (0.58254019f)
#define COEF_B1   (0.20872991f)


class ImageHFilter {

  public:

    const unsigned width_out;
    const unsigned input_stride;
    const unsigned coef_offset;
    const unsigned black_level;

    int32_t acc_init;
    int8_t coef[32];
    unsigned shift;

    ImageHFilter(
        const unsigned width_out,
        const unsigned input_stride,
        const unsigned coef_offset,
        const int black_level)
          : width_out(width_out), 
            input_stride(input_stride), 
            coef_offset(coef_offset),
            black_level(black_level) {}

    void filter(
        int8_t output[],
        const int8_t input[]);

    void update_gain(
        const float gain);

};




void ImageHFilter::filter(
    int8_t output[],
    const int8_t input[]) 
{
  pixel_hfilter(&output[0],
                &input[0],
                &coef[0],
                acc_init,
                shift,
                this->input_stride,
                this->width_out);
}


void ImageHFilter::update_gain(
    const float gain)
{
  float sc_b0 = COEF_B0 * gain;
  float sc_b1 = COEF_B1 * gain;

  // Faster than computing ceil(log2(__))
  if(sc_b0 <= 0.25f)      this->shift = 9;
  else if(sc_b0 <= 0.5f)  this->shift = 8;
  else if(sc_b0 <= 1.0f)  this->shift = 7;
  else if(sc_b0 <= 2.0f)  this->shift = 6;
  else                    this->shift = 5;

  const int shift_scale = 1 << this->shift;

  const float b0 = (sc_b0 * shift_scale);
  const float b1 = (sc_b1 * shift_scale);

  const int8_t b0_s8 = (b0 >= INT8_MAX) ? INT8_MAX : (b0 + 0.5f);
  const int8_t b1_s8 = (b1 >= INT8_MAX) ? INT8_MAX : (b1 + 0.5f);

  const unsigned s = this->coef_offset;

  this->coef[0+s] = this->coef[4+s] = b1_s8;
  this->coef[2+s] = b0_s8;

  const float sum_b = b0 + 2*b1;

  this->acc_init = (128 * (sum_b - shift_scale) - this->black_level * shift_scale);
}



#endif // __cplusplus