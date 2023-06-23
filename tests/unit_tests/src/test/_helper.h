#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Print formatting
#define print_separator(x) printf("\n---------- %s -------------\n", x)

// Fill array
void fill_array_rand_int8(int8_t *image, size_t size);
void fill_array_rand_uint8(uint8_t *image, size_t size);
