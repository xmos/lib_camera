#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include <print.h>
#include "camera_isp.h"

// ------ kernels ------
#define WB_KR 44   
#define WB_KG 0
#define WB_KB 44
#define WB_CR 6
#define WB_CG 4
#define WB_CB 6
#define WB_SAT 2

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

const int32_t vc_red[3] = {WB_CR >> WB_SAT, WB_CG >> WB_SAT, WB_CB >> WB_SAT};
const int32_t vk_red[3] = {WB_KR, WB_KG, WB_KB};

// ------ ASM ------
extern void wb_scale(
    int8_t px[32],
    const int8_t vk[32], 
    const int8_t vc[32],
    const int16_t vsat[16]
);

extern void wb_scale_image(
    int8_t *img,
    unsigned img_size,
    const int8_t vk[32], 
    const int8_t vc[32],
    const int16_t vsat[16]
);

// ------------------------------------
static inline 
void wb_scale_tail(
    int8_t *img,
    unsigned img_size)
{
    // Compute the 32 elements tail
    unsigned tail = img_size - 32;
    unsigned pos = tail % 3;
    for (unsigned i = tail; i < img_size; i++){
        int32_t tmp = (int32_t)img[i];
        tmp = (vc_red[pos]*tmp + vc_red[pos]*vk_red[pos]);
        tmp = tmp > INT8_MAX ? INT8_MAX : tmp;
        tmp = tmp < INT8_MIN ? INT8_MIN : tmp;
        img[i] = (int8_t) (tmp);
        pos = (pos + 1) % 3;
    }
}

void camera_isp_white_balance(image_cfg_t* image)
{
    assert(image->ptr != NULL && "image pointer is NULL");
    wb_scale_image(image->ptr, image->size, vk_rgb, vc_rgb, vsat);
    wb_scale_tail(image->ptr, image->size);
}
