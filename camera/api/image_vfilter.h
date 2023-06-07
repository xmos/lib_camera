#ifndef IMAGE_VFILTER_H
#define IMAGE_VFILTER_H

#include <stdint.h>

#include "sensor.h"



#define VFILTER_TAP_COUNT   (5)

#define VFILTER_DEC_FACTOR  (APP_DECIMATION_FACTOR / 2)

// Required: Low-res image width must be multiple of 16 pixels
#if (((APP_IMAGE_WIDTH_PIXELS)>>4)<<4) != (APP_IMAGE_WIDTH_PIXELS)
# error MIPI_IMAGE_LORES_WIDTH_PIX (width of decimated image) must be multiple of 16 pixels.
#endif

// The number of accumulators required for vertical filtering.
// ceil(tap_count / dec_factor)
#define VFILTER_ACC_COUNT  ((VFILTER_TAP_COUNT + VFILTER_DEC_FACTOR - 1)\
                              / (VFILTER_DEC_FACTOR))

// The value that the next_tap gets reset to after outputting a row.
#define VFILTER_RESET_INDEX   ((VFILTER_TAP_COUNT) - \
                  ( (VFILTER_DEC_FACTOR) * (VFILTER_ACC_COUNT) ))

#define VFILTER_ACC_WIDTH_SHORTS  (2*APP_IMAGE_WIDTH_PIXELS)



#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif


typedef struct {
  int next_tap;
  int16_t buff[VFILTER_ACC_WIDTH_SHORTS];
} vfilter_acc_t;


void pixel_vfilter_acc_init(
    int16_t* accs,
    const int32_t acc_value,
    const unsigned pix_count);

void pixel_vfilter_complete(
    int8_t* pix_out,
    const int16_t* accs,
    const int16_t shifts[16],
    const unsigned pix_count);

void pixel_vfilter_macc(
    int16_t* accs,
    const int8_t* pix_in,
    const int8_t filter[16],
    const unsigned length_bytes);


void image_vfilter_frame_init(
    vfilter_acc_t accs[]);

unsigned image_vfilter_process_row(
    int8_t output[],
    vfilter_acc_t acc[],
    const int8_t pixel_data[]);

unsigned image_vfilter_drain(
    int8_t output[],
    vfilter_acc_t acc[]);

#if defined(__XC__) || defined(__cplusplus)
}
#endif

#endif //IMAGE_VFILTER_H
