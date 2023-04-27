#pragma once

#include <stdint.h>
#include "mipi_main.h"

#ifdef __XC__
extern "C" {
#endif

void write_image(uint8_t *image);
void c_memcpy(void *dst, void *src, size_t size);
void process_image(uint8_t *image);
// void user_input(void);

#ifdef __XC__
}
#endif