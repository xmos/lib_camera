#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __XC__
extern "C" {
#endif

#include "xscope_io_device.h"

void swap_dimentions(uint8_t * image_in, uint8_t * image_out, const size_t height, const size_t width, const size_t channels);

void write_image_file(char * filename, uint8_t * image, const size_t height, const size_t width, const size_t channels);

void write_bmp_file(char * filename, uint8_t * image, const size_t height, const size_t width, const size_t channels);

#ifdef __XC__
}
#endif
