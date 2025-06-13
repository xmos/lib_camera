// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <debug_print.h>
#include <xcore/assert.h>
#include <xcore/chanend.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"

#include "kernels_yuv.h"

// defined in raw8_block_to_yuv422.S
extern void raw8_block_to_yuv422(
    int8_t *dst,
    int8_t *src,
    unsigned line_size
);

static inline 
void vpu_set_int8_mode(){
    asm volatile("vclrdr");
    asm volatile("ldc r11, 0x200");
    asm volatile("vsetc r11");
}

static
void block_raw8_to_yuv422_vpu(
    int8_t *out_ptr, 
    int8_t input_rows[2][MODE_YUV2_MAX_SIZE], 
    unsigned img_width)
{    
    const unsigned steps = 16;
    const unsigned loop_size = ((img_width << 1) - 4);
    const unsigned line_size = MODE_YUV2_MAX_SIZE;
    vpu_set_int8_mode();
    for (unsigned x = 0; x <= (loop_size); x += steps) {
        int8_t *src = (int8_t *)&input_rows[0][x];
        int8_t *dst = &out_ptr[x];
        raw8_block_to_yuv422(dst, src, line_size);
    }
}

void camera_isp_raw8_to_yuv2(image_cfg_t* image, int8_t* data_in, unsigned sensor_ln){
    unsigned x1 = image->config->x1;
    unsigned y1 = image->config->y1;
    unsigned img_width = image->width;
    unsigned img_channels = image->channels;
    int8_t* data_src = data_in + x1;
    int8_t* img_ptr = image->ptr;

    static int8_t input_rows[2][MODE_YUV2_MAX_SIZE] ALIGNED_8 = { {0} };
    unsigned buff_ln = sensor_ln % 2;
    xmemcpy(&input_rows[buff_ln][0], data_src, img_width * 2);
    if(buff_ln == 1) {
        unsigned img_ln = (sensor_ln - y1 - 1) >> 1;
        int8_t *out_ptr = img_ptr + ((img_ln * img_width)) * (img_channels);
        block_raw8_to_yuv422_vpu(out_ptr, input_rows, img_width);
    }
}
