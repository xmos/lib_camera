// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <xscope.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <xcore/parallel.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_io.h"
#include "camera_conv.h"

#define WB_gain_RED    1.538
#define WB_gain_GREEN  1.0
#define WB_gain_BLUE   1.587

static
void fill_img_rand_int8(image_cfg_t* image)
{
    int8_t* data = image->ptr;
    unsigned size = image->size;
    for (unsigned idx = 0; idx < size; idx++) {
        data[idx] = (int8_t)((rand() % 256) - 128);
    }
}

static inline
void plain_wb(int8_t* arr, unsigned arr_size){
    float pfilters[3] = {
        WB_gain_RED,
        WB_gain_GREEN,
        WB_gain_BLUE
    };
    unsigned pos = 0;
    for (unsigned i = 0; i < arr_size; i++)
    {
        float tmp = pfilters[pos] * ((float)arr[i] + 128) - 128;
        tmp = tmp > INT8_MAX ? INT8_MAX : tmp;
        tmp = tmp < INT8_MIN ? INT8_MIN : tmp;
        arr[i] = (int8_t) tmp;
        pos = (pos + 1) % 3;
    }
}

int main()
{
    // Image and configuration
    const unsigned h = 64;
    const unsigned w = 64;
    const unsigned ch = 3;
    const unsigned img_size = h * w * ch;
    int8_t image_buffer[img_size] = { 0 };
    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_RGB1,
    };
    image_cfg_t image = {
        .height = h,
        .width = w,
        .channels = ch,
        .size = h*w*ch,
        .ptr = &image_buffer[0],
        .config = &config
    };
    camera_isp_coordinates_compute(&image);
    fill_img_rand_int8(&image);

    // Print the first 10 values
    printf("\nFirst 10 values of the image before SWB:\n");
    for (unsigned i = 0; i < 10; i++) {
        printf("%d ", image_buffer[i]);
    }
    
    // do awb
    unsigned ta = 0, tb = 0;
    ta = get_reference_time();
    camera_isp_white_balance(&image);
    tb = get_reference_time();
    printf("\nTime taken for SWB: %d cycles\n", tb - ta);

    // print the first 10 values
    printf("\nFirst 10 values of the image after SWB:\n");
    for (unsigned i = 0; i < 10; i++) {
        printf("%d ", image_buffer[i]);
    }
    return 0;
}

// xgdb bin/test_isp_swb.xe -ex "connect --xscope" -ex "load" -ex "break wb_scale_image"
//
