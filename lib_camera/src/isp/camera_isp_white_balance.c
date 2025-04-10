// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xcore/assert.h>

#include <print.h>
#include "camera_isp.h"

// ------ kernels ------
#define WB_KR 45   
#define WB_KG 0
#define WB_KB 47

#define WB_CR 12
#define WB_CG 8
#define WB_CB 13
#define WB_SAT 3


const int8_t vk_rgb[32] ALIGNED_4 = {
    WB_KR, WB_KG, WB_KB, 
    WB_KR, WB_KG, WB_KB, 
    WB_KR, WB_KG, WB_KB,
    WB_KR, WB_KG, WB_KB, // 12
    0, 0, 0, 0, // 16
    0, 0, 0, 0, // 20
    0, 0, 0, 0, // 24
    0, 0, 0, 0, // 28
    0, 0, 0, 0, // 32
};

const int8_t vc_rgb[32] ALIGNED_4 = {
    WB_CR, WB_CG, WB_CB,
    WB_CR, WB_CG, WB_CB,
    WB_CR, WB_CG, WB_CB,
    WB_CR, WB_CG, WB_CB, // 12
    0, 0, 0, 0, // 16
    0, 0, 0, 0, // 20
    0, 0, 0, 0, // 24
    0, 0, 0, 0, // 28
    0, 0, 0, 0, // 32
};

const int16_t vsat[16] ALIGNED_4 = {
    WB_SAT,WB_SAT,WB_SAT,WB_SAT,
    WB_SAT,WB_SAT,WB_SAT,WB_SAT,
    WB_SAT,WB_SAT,WB_SAT,WB_SAT,
    WB_SAT,WB_SAT,WB_SAT,WB_SAT
};

extern void wb_scale_image(
    image_cfg_t* image,
    const int8_t vk[32], 
    const int8_t vc[32],
    const int16_t vsat[16]
);

// ------------------------------------

void camera_isp_white_balance(image_cfg_t* image)
{
    xassert(image->ptr != NULL && "image pointer is NULL");
    xassert(image->size > 12 && "image size should be > 12 bytes");
    wb_scale_image(image, vk_rgb, vc_rgb, vsat);
}
