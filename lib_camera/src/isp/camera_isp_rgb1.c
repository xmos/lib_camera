// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>

#include <debug_print.h>
#include <xcore/assert.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"

#include "kernels_rgb.h"


extern void demosaic_4raw_2rgb(
    int8_t * img_ptr, int8_t * tmp_buff, 
    unsigned img_ln, unsigned img_width, unsigned tmp_width);

void camera_isp_raw8_to_rgb1(image_cfg_t* image, int8_t* data_in, unsigned sensor_ln) {
    unsigned x1 = image->config->x1; //TODO create a logic to check only if needed
    unsigned y1 = image->config->y1;
    unsigned img_width = image->width;
    unsigned img_channels = image->channels;
    xassert(img_channels == 3);
    int8_t* data_src = data_in + x1;

    // 4 rows of 200 pixels
    static int8_t input_rows[4][MODE_RGB1_MAX_SIZE] ALIGNED_8 = { {0} };

    // if even, move data, if odd compute
    unsigned ln_is_even = (sensor_ln % 2 == 0);
    if (ln_is_even) {
        xmemcpy(&input_rows[0][0], &input_rows[2][0], img_width);     // move [2][x] to [0][x]
        xmemcpy(&input_rows[1][0], &input_rows[3][0], img_width);     // move [3][x] to [1][x]
        xmemcpy(&input_rows[2][0], data_src, img_width);              // move new data to [2][x]
    }
    else{ // if odd
        unsigned img_ln = sensor_ln - y1 - 1;        
        int8_t *img_ptr = image->ptr;

        xmemcpy(&input_rows[3][0], data_src, img_width);              // move new data to [3][x]

        // img_ptr, buff, img_ln, img_width, tmp_width
        demosaic_4raw_2rgb(img_ptr, &input_rows[0][0], img_ln, img_width, MODE_RGB1_MAX_SIZE);
    }
}
