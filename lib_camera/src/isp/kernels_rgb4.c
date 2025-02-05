// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>

#include <kernels_rgb.h>

const int16_t kernel_rgb4_shifts[16] __attribute__((aligned(4))) = {
    3,3,3,3,
    3,3,3,3,
    3,3,3,3,
    3,3,3,3,
};

// info 
// kernel order is: 1,0

// Due to VPU shifting, kernels are in BGR order so they will produce RGB pixels
const int8_t kernels_rgb4[] __attribute__((aligned(4))) = {
// kernel_rgb4_blue1
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 2,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 2, 0, 2,
// kernel_rgb4_green1
    0, 0, 0, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0, 1,
    0, 0, 0, 0, 1, 0, 1, 0,
// kernel_rgb4_red1
    0, 0, 0, 0, 2, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 2, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
// kernel_rgb4_blue0
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 2, 0, 0, 0, 0,
// kernel_rgb4_green0
    0, 1, 0, 1, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 0,
    0, 1, 0, 1, 0, 0, 0, 0,
    1, 0, 1, 0, 0, 0, 0, 0,
// kernel_rgb4_red0
    2, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 0, 2, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
};
