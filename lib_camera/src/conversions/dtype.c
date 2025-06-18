// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>

#include <print.h>
#include <xcore/assert.h>

#include "camera_conv.h"

// defined in dtype_int8_to_uint8.S
extern 
void camera_int8_to_uint8_block(uint8_t* out, int8_t* in,const unsigned len);

void camera_int8_to_uint8(
  uint8_t* output,
  int8_t* input,
  const unsigned length)
{
  for (unsigned k = 0; k < length; k++) {
    output[k] = input[k] + 128;
  }
}

void camera_int8_to_uint8_fast(
  uint8_t* output,
  int8_t* input,
  const unsigned length)
{
  xassert(length >= 8);
  xassert((uintptr_t)output % 8 == 0);
  xassert((uintptr_t)input % 8 == 0);
  camera_int8_to_uint8_block(output, input, length);
}
