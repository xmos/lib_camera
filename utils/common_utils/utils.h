#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef __XC__
extern "C" {
#endif

#include "xscope_io_device.h"

void write_image(char * filename, uint8_t * image, const size_t height, const size_t width);
void not_silly_memcpy(void * dst, void * src, size_t size);

#ifdef __XC__
}
#endif
