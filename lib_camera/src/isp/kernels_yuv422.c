// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>

#include "kernels_yuv.h"

// kernels (original, BT.601 in fixed point)
/*
static const int8_t a = 38, b = 75, c = 14;
static const int8_t d = -21, e = -42, f = 64;
static const int8_t g = 64, h = -53, i = -10;
*/

// kernels (with wb)
static const int8_t a = 47, b = 61, c = 18;
static const int8_t d = -27, e = -34, f = 82;
static const int8_t g = 80, h = -43, i = -13;

// offsets (wb)
static const int8_t yk = 0;
static const int8_t uk = 21;
static const int8_t vk = 23;

const int8_t kernels_yuv422[16*32] ALIGNED_4 = {
    // kernels_yuv_v6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, g, h, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, i, 0, 0,
    // kernels_yuv_y7
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, c,
    // kernels_yuv_u6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, d, e, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, f, 0, 0,
    // kernels_yuv_y6
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, 0, 0,   
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, c, 0, 0,
    // kernels_yuv_v4
    0, 0, 0, 0, 0, 0, 0, 0, g, h, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0, 0, 0, 0, 0, i, 0, 0, 0, 0, 0, 0, 
    // kernels_yuv_y5
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, a, b, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, c, 0, 0, 0, 0,  
    // kernels_yuv_u4
    0, 0, 0, 0, 0, 0, 0, 0, d, e, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0, 0, 0, 0, 0, f, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_y4
    0, 0, 0, 0, 0, 0, 0, 0, a, b, 0, 0, 0, 0, 0, 0,  
    0, 0, 0, 0, 0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_v2
    0, 0, 0, 0, g, h, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    // kernels_yuv_y3
    0, 0, 0, 0, 0, 0, a, b, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, 
    // kernels_yuv_u2
    0, 0, 0, 0, d, e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_y2
    0, 0, 0, 0, a, b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_v0,
    g, h, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_y1, 
    0, 0, a, b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_u0, 
    d, e, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    // kernels_yuv_y0,
    a, b, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, c, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const int16_t kernels_yuv422_sat[16] ALIGNED_4 = {
    7,7,7,7,
    7,7,7,7,
    7,7,7,7,
    7,7,7,7,
};

const int8_t kernels_yuv422_adds[32] ALIGNED_4 = {
    yk, uk, yk, vk,
    yk, uk, yk, vk,
    
    yk, uk, yk, vk,
    yk, uk, yk, vk,

    yk, uk, yk, vk,
    yk, uk, yk, vk,

    yk, uk, yk, vk,
    yk, uk, yk, vk,
};
