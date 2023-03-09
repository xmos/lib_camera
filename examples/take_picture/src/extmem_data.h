
#pragma once

#include <stdint.h>

#include "mipi.h"
#include "mipi_main.h"

#ifdef __XC__
extern "C" {
#endif

extern
uint8_t image_capture[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_IMAGE_WIDTH_PIXELS][2];

void not_silly_memcpy(
  void* dst,
  void* src,
  size_t size);

void write_image();

#ifdef __XC__
}
#endif