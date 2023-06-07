
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "image_hfilter.h"
#include "isp.h"

// The filter coefficients for the horizontal filter. Because the VPU (and in
// particular, VLMACCR) is used, this needs to be 32 bytes long, padded with
// zeros where necessary.

// VPU loads must be 4-byte-aligned.  So when filtering e.g. a bayered RGGB
// image we can't filter red, move up one byte in the pixel row, and apply the
// same filter again. Instead we need a pair of filters, containing the same
// coefficients, but offset by one index. Then, when horizontally filtering the
// G channel we provide the filter function the SAME (word-aligned) start
// address that we provided with the R channel, but using the offset
// coefficients so that the R channel is ignored instead of the G channel.
// Additionally, note that to avoid mixing channels, every other coefficient
// must be zero, in each case.
#define HFILTER_INPUT_STRIDE  (APP_DECIMATION_FACTOR)

//Note: for filter coefficients reference : python/filters.txt



void image_hfilter_update_scale(
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

  state->acc_init = (128 * (sum_b - shift_scale));
}



void image_hfilter(
    int8_t pix_out[APP_IMAGE_WIDTH_PIXELS],
    const hfilter_state_t* state,
    const int8_t pix_in[SENSOR_RAW_IMAGE_WIDTH_PIXELS])
{
  pixel_hfilter(&pix_out[0],
                &pix_in[0],
                &state->coef[0],
                state->acc_init,
                state->shift,
                HFILTER_INPUT_STRIDE,
                APP_IMAGE_WIDTH_PIXELS);
}
