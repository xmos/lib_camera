#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

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

#ifndef ALIGNED_4
#define ALIGNED_4 __attribute__((aligned(4)))
#endif

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
    assert(image->ptr != NULL && "image pointer is NULL");
    assert(image->size > 12 && "image size should be > 12 bytes");
    wb_scale_image(image, vk_rgb, vc_rgb, vsat);
}
