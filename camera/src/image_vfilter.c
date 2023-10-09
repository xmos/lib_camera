// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

//Note: for filter coefficients reference : python/filters.txt

#include <stdint.h>
#include <stdio.h>

#include "image_vfilter.h"

static
const int32_t vfilter_acc_offset = 0;

static
const int8_t vfilter_coef[5][16] = {
  {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,},
  { 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,},
  {114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,114,},
  { 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,},
  {  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,},
};

static
const int16_t vfilter_shift[16] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,};

/**
 * Prepare the provided accumulator struct for accumulation.
 */
static inline
void image_vfilter_reset(
    vfilter_acc_t* acc)
{
  acc->next_tap = VFILTER_RESET_INDEX;
  pixel_vfilter_acc_init(acc->buff, vfilter_acc_offset, 
                         APP_IMAGE_WIDTH_PIXELS);
}


void image_vfilter_frame_init(
    vfilter_acc_t accs[])
{
  for(int k = 0; k < VFILTER_ACC_COUNT; k++){
    accs[k].next_tap = -(VFILTER_DEC_FACTOR) * k + (VFILTER_TAP_COUNT/2);
    pixel_vfilter_acc_init(accs[k].buff, vfilter_acc_offset, 
                           APP_IMAGE_WIDTH_PIXELS);
  }
}


unsigned image_vfilter_process_row(
    int8_t output[],
    vfilter_acc_t acc[],
    const int8_t pixel_data[])
{
  for(int k = 0; k < VFILTER_ACC_COUNT; k++){
    if(acc[k].next_tap >= 0){
      pixel_vfilter_macc(acc[k].buff,
                         pixel_data,
                         &vfilter_coef[acc[k].next_tap][0],
                         APP_IMAGE_WIDTH_PIXELS);
    }
    acc[k].next_tap++;
  }

  for(int k = 0; k < VFILTER_ACC_COUNT; k++){
    if(acc[k].next_tap != VFILTER_TAP_COUNT) continue;

    // produce an output row from accumulator
    pixel_vfilter_complete(output,
                            acc[k].buff,
                            vfilter_shift,
                            APP_IMAGE_WIDTH_PIXELS);

    // reset the accumulator
    image_vfilter_reset(&acc[k]);

    return 1;
  }

  return 0;
}

unsigned image_vfilter_drain(
    int8_t output[],
    vfilter_acc_t acc[])
{
  for (int k = 0; k < VFILTER_ACC_COUNT; k++) {
    if (acc[k].next_tap <= 0)
      continue;

    pixel_vfilter_complete(
      output,
      acc[k].buff,
      vfilter_shift,
      APP_IMAGE_WIDTH_PIXELS);
    
    acc[k].next_tap = 0;

    return 1;
  }

  return 0;
}
