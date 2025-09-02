// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stdlib.h>

#ifndef ALIGNED_4
#define ALIGNED_4 __attribute__((aligned(4)))
#endif

extern const int8_t kernels_yuv422[16*32];
extern const int16_t kernels_yuv422_sat[16];
extern const int8_t kernels_yuv422_adds[32];
