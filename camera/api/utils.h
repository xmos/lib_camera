#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "sensor.h"

#define PRINT_TIME(a,b) printf("%d\n", b - a);

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

static inline
int measure_time(){
  int y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}

void write_image(
    const char* filename,
    uint8_t image[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);
void c_memcpy(void *dst, void *src, size_t size);
void writeBMP(const char *filename, uint8_t img[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);
void write_image_raw(const char* filename,uint8_t *image);


#if defined(__XC__) || defined(__cplusplus)
}
#endif