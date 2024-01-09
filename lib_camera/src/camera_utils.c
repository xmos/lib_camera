// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera_utils.h"

inline unsigned measure_time()
{
  unsigned y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}

void vect_int8_to_uint8(
    uint8_t output[],
    int8_t input[],
    const size_t length)
{
  for (size_t k = 0; k < length; k++)
    output[k] = input[k] + 128;
}
