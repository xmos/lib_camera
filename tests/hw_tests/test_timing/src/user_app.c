// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xcore/channel.h>
#include <xcore/select.h>

#include <xscope.h>

#include "camera.h"
#include "camera_utils.h"
#include "camera_isp.h"
#include "camera_conv.h"
#include "camera_io.h"

#define DELAY_MILISECONDS 100
#define FILE1_NAME "capture1.rgb"
#define FILE2_NAME "capture2.rgb"


extern void camera_isp_get_capture_xscope(chanend_t c_cam);
extern void camera_isp_start_capture_xscope(chanend_t c_cam, image_cfg_t *image);


static
void save_image(image_cfg_t* image, char* filename) {
    uint8_t * img_ptr = (uint8_t*)image->ptr;
    camera_io_write_image_file(filename, img_ptr, image->height, image->width, image->channels); // this will close the file as well
}

void user_app(chanend_t c_cam) {
    // Image and configuration
    const unsigned h = 200;
    const unsigned w = 200;
    const unsigned ch = 3;
    const unsigned img_size = h * w * ch;
    int8_t image_buffer[img_size] = { 0 };

    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_RGB2,
    };
    image_cfg_t image = {
        .height = h,
        .width = w,
        .channels = ch,
        .size = h*w*ch,
        .ptr = &image_buffer[0],
        .config = &config
    };

    // wait a few seconds and ask somthing
    delay_milliseconds_cpp(300);

    // compute the image coordinates
    camera_isp_coordinates_compute(&image);
    
    const uint32_t n_captures = 10;
    uint32_t t_start[n_captures];
    uint32_t t_end[n_captures];

    for (unsigned i = 0; i < n_captures; i++) {
        t_start[i] = get_reference_time();
        camera_isp_start_capture_xscope(c_cam, &image);
        camera_isp_get_capture_xscope(c_cam);
        t_end[i] = get_reference_time();
    }
    
    for (unsigned i = 0; i < n_captures; i++) {
        printf("Capture %d time:\t%f [ms]\n", i, (t_end[i] - t_start[i]) * 1e-5);
    }
    
    save_image(&image, FILE2_NAME);
    exit(0);
}
