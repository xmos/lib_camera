
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "image_hfilter.h"


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

// float hfilter_coef[3] = { 0.20872991, 0.58254019, 0.20872991 };

#define A (0x1B)
#define B (0x4B)
#define C (0x64)

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


//#define RED_GAIN    1
//#define GREEN_GAIN  1
//#define BLUE_GAIN   1

#define RED_GAIN    1//1.3
#define GREEN_GAIN  0.8//1
#define BLUE_GAIN   1//1.3

int8_t hfilter_red[32] = {
  0x1B,0x00,0x4B,0x00,0x1B,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
int8_t hfilter_green[32] = {
  0x1B,0x00,0x4B,0x00,0x1B,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
int8_t hfilter_blue[32] = {
  0x1B,0x00,0x4B,0x00,0x1B,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


/// ----------------------------------------------------------------
const int16_t hfilter_acc_init[2][16] = {
  { 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, },
  { 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
    0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000, },
};
const int16_t hfilter_shift[16] = {7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7};


int8_t compute_gain(int8_t num, float f) {
  float result = f * (num + 127.0f);
  if (result >= 255.0f) {
    return 127;  
  } else if (result <= 0.0f) {
    return -127;
  }
  result -= 127.0f;
  return result;
}


void image_hfilter(
    int8_t pix_out[APP_IMAGE_WIDTH_PIXELS],
    const int8_t pix_in[SENSOR_RAW_IMAGE_WIDTH_PIXELS],
    const unsigned channel_index)
{
  // modify filters
  hfilter_red[0] = compute_gain(A, RED_GAIN);
  hfilter_red[2] = compute_gain(B, RED_GAIN);
  hfilter_red[4] = compute_gain(A, RED_GAIN);
  
  hfilter_green[1] = compute_gain(A, GREEN_GAIN);
  hfilter_green[3] = compute_gain(B, GREEN_GAIN);
  hfilter_green[5] = compute_gain(A, GREEN_GAIN);

  hfilter_blue[1] = compute_gain(A, BLUE_GAIN);
  hfilter_blue[3] = compute_gain(B, BLUE_GAIN);
  hfilter_blue[5] = compute_gain(A, BLUE_GAIN);

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
