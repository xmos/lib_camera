// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xcore/channel.h>
#include <xcore/select.h>

#include "camera.h"
#include "camera_utils.h"
#include "camera_isp.h"
#include "camera_conv.h"
#include "camera_io.h"

static
void save_image(image_cfg_t* image, char* filename) {
    uint8_t * img_ptr = (uint8_t*)image->ptr;
    camera_io_write_image_file(
        filename, 
        img_ptr, 
        image->height, 
        image->width, 
        image->channels);
}


void user_app(chanend_t c_cam) {
    // Image and configuration
    const unsigned h = 480;
    const unsigned w = 640;
    const unsigned ch = 1;
    const unsigned img_size = h * w * ch;
    int8_t image_buffer[img_size] = { 0 };
    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_RAW,
    };
    image_cfg_t image = {
        .height = h,
        .width = w,
        .channels = ch,
        .size = h*w*ch,
        .ptr = &image_buffer[0],
        .config = &config
    };

    // wait a few seconds and ask something
    delay_seconds_cpp(3);
        
    // set coords and send to ISP
    for (camera_patterns_t tp = 0; tp <= PATTERN_PN_31; tp++) {
        camera_sensor_set_tp(tp);
        camera_isp_coordinates_compute(&image);
        camera_isp_start_capture(c_cam, &image);
        camera_isp_get_capture(c_cam);
        char filename[50];
        sprintf(filename, "capture_tp_%d.raw", tp);
        save_image(&image, filename);
        puts("capture done\n");
    }
    exit(0);
}
