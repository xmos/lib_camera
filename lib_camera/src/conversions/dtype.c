#include <stdint.h>

#include <print.h>

#include "camera_conv.h"

void camera_conv_int8_to_uint8(
    uint8_t output[],
    int8_t input[],
    const unsigned length)
{
  for (unsigned k = 0; k < length; k++)
    output[k] = input[k] + 128;
}
