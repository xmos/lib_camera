// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>

#include <debug_print.h>
#include <xcore/assert.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"

extern void block_ds_4ln_raw8_to_rgb(
    int8_t * img_ptr, int8_t * tmp_buff, 
    unsigned img_ln, unsigned img_width, unsigned tmp_width);

void camera_isp_raw8_to_rgb4(image_cfg_t* image, int8_t* data_in, unsigned sensor_ln){
    unsigned x1 = image->config->x1; //TODO create a logic to check only if needed
    unsigned y1 = image->config->y1;
    unsigned img_width = image->width;
    unsigned img_channels = image->channels;
    xassert(img_channels == 3);
    int8_t* data_src = data_in + x1;

    // 4 rows of 800 pixels
    static int8_t input_rows[4][MODE_RGB4_MAX_SIZE] ALIGNED_8 = { {0} };

    unsigned buff_ln = sensor_ln % 4;

    xmemcpy(&input_rows[buff_ln][0], data_src, img_width * 4);

    if(buff_ln == 3) {
        unsigned img_ln = sensor_ln - y1 - 3;
        int8_t *img_ptr = image->ptr;

        block_ds_4ln_raw8_to_rgb(img_ptr, &input_rows[0][0], img_ln, img_width, MODE_RGB4_MAX_SIZE);
    }
}
