
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "camera_utils.h"


inline unsigned measure_time()
{
  unsigned y = 0;
  asm volatile("gettime %0"
               : "=r"(y));
  return y;
}

void vect_int8_to_uint8(
    uint8_t output[],
    int8_t input[],
    const unsigned length)
{
  for (int k = 0; k < length; k++)
    output[k] = input[k] + 128;
}
