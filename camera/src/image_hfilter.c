
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

#define A (0x1B)
#define B (0x4B)

#define C (0x1B)
#define D (0x4B)

/*
const int8_t hfilter_coef_bayered_even[32] = {
  0x1B,0x00,0x4B,0x00,0x1B,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const int8_t hfilter_coef_bayered_odd[32] = {
  0x00,0x1B,0x00,0x4B,0x00,0x1B,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
*/

#define AWB_gain_RED    1.3
#define AWB_gain_BLUE   0.8
#define AWB_gain_GREEN  1.3

int8_t hfilter_red[32]   = {
  C,0x00,D,0x00,C,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

int8_t hfilter_green[32] = {
  0x00,A,0x00,B,0x00,A,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

int8_t hfilter_blue[32]  = {
  0x00,C,0x00,D,0x00,C,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


/// ----------------------------------------------------------------
const int16_t MIN   = 0x0000;
const int16_t MIN2  = 0x0000;

const int16_t hfilter_acc_init[2][16] = {
  { MIN,MIN,MIN,MIN,MIN,MIN,MIN,MIN,
    MIN,MIN,MIN,MIN,MIN,MIN,MIN,MIN, },
  { MIN2,MIN2,MIN2,MIN2,MIN2,MIN2,MIN2,MIN2,
    MIN2,MIN2,MIN2,MIN2,MIN2,MIN2,MIN2,MIN2, },
};

const int16_t N = 7;
const int16_t hfilter_shift[16] = {N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N};

void apply_gains(
  int8_t gains[3], 
  int8_t *filter,
  const int8_t offset)
{
  filter[0+offset] = gains[0];
  filter[2+offset] = gains[1];
  filter[4+offset] = gains[2];
}


void image_hfilter(
    int8_t pix_out[APP_IMAGE_WIDTH_PIXELS],
    const int8_t pix_in[SENSOR_RAW_IMAGE_WIDTH_PIXELS],
    const unsigned channel_index)
{
  // appply gains
  // apply_gains({0x1B, 0x4B, 0x1B}, &hfilter_red[0], 0);
  // apply_gains({0x1B, 0x4B, 0x1B}, &hfilter_green[0], 0);
  // apply_gains({0x1B, 0x4B, 0x1B}, &hfilter_blue[0], 0);

  // group filters
  int8_t* channel_hfilter_coefs_rggb[3] = {
    &hfilter_red[0],
    &hfilter_green[0],
    &hfilter_blue[0],
  };

  assert(channel_index >= 0 && channel_index <= 2);

  pixel_hfilter(&pix_out[0],
                &pix_in[0],
                channel_hfilter_coefs_rggb[channel_index],
                &hfilter_acc_init[0][0],
                &hfilter_acc_init[1][0],
                hfilter_shift,
                HFILTER_INPUT_STRIDE,
                APP_IMAGE_WIDTH_PIXELS);
}
