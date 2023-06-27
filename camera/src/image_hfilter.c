#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "image_hfilter.h"
#include "isp.h"

//Note: for filter coefficients reference : python/filters.txt

void pixel_hfilter_update_scale(
    hfilter_state_t* state,
    const float gain,
    const unsigned offset)
{
  float sc_b0 = COEF_B0 * gain;
  float sc_b1 = COEF_B1 * gain;

  // Faster than computing ceil(log2(__))
  if(sc_b0 <= 0.25f)      state->shift = 9;
  else if(sc_b0 <= 0.5f)  state->shift = 8;
  else if(sc_b0 <= 1.0f)  state->shift = 7;
  else if(sc_b0 <= 2.0f)  state->shift = 6;
  else                    state->shift = 5;

  const int shift_scale = 1 << state->shift;

  const float b0 = (sc_b0 * shift_scale);
  const float b1 = (sc_b1 * shift_scale);

  const int8_t b0_s8 = (b0 >= INT8_MAX) ? INT8_MAX : (b0 + 0.5f);
  const int8_t b1_s8 = (b1 >= INT8_MAX) ? INT8_MAX : (b1 + 0.5f);

  const unsigned s = offset;

  state->coef[0+s] = state->coef[4+s] = b1_s8;
  state->coef[2+s] = b0_s8;

  const float sum_b = b0 + 2*b1;

  state->acc_init = 128 * (sum_b - shift_scale) - SENSOR_BLACK_LEVEL * shift_scale;
}
