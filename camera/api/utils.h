#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "sensor.h"

#define PRINT_TIME(a,b) printf("%d\n", b - a);

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

static inline
unsigned measure_time(){
  unsigned y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}

void c_memcpy(void *dst, void *src, size_t size);
void rotate_image(const char *filename, uint8_t image[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);

void img_int8_to_uint8(
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS], 
  uint8_t out_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]
);

void img_int8_to_uint8_inplace(int8_t image_buffer[H_RAW][W_RAW]);

#if defined(__XC__) || defined(__cplusplus)
}
#endif
