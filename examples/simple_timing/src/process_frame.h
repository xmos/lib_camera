#pragma once

#include <stdint.h>
#include "mipi_main.h"

#ifdef __XC__
extern "C" {
#endif

void write_image();
void not_silly_memcpy(void *dst, void *src, size_t size);
// void user_input(void);

#ifdef __XC__
}
#endif