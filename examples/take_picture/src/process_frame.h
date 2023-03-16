#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __XC__
extern "C" {
#endif

#define MIPI_IMAGE_WIDTH_PIXELS     800 // csi2 packed   
#define MIPI_IMAGE_HEIGHT_PIXELS    480
#define PIXEL_MULTIPLIER 1          // because is RAW 10, 1 byte per pyxel
#define MIPI_LINE_WIDTH_BYTES ((MIPI_IMAGE_WIDTH_PIXELS)*PIXEL_MULTIPLIER) // because RAW 10


extern uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES];

void write_image();
void not_silly_memcpy(
    void *dst,
    void *src,
    size_t size);

#ifdef __XC__
}
#endif