#pragma once

#include <stdint.h>
#include "mipi_main.h"

#ifdef __XC__
extern "C" {
#endif

// Store the image
extern uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES];

void write_image();
void not_silly_memcpy(void *dst, void *src, size_t size);
// void user_input(void);

#ifdef __XC__
}
#endif