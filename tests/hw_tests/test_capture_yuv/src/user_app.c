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

#define FILE_NAME "capture_yuv.bin"

static inline
void save_image(image_cfg_t* image, char* filename)
{
    camera_io_write_image_file(
        filename,
        (uint8_t*)image->ptr,
        image->height,
        image->width,
        image->channels);
    printstr("Image saved to file: ");
    printstrln(filename);
}

void user_app(chanend_t c_cam)
{

    // Image and configuration
    const unsigned h = 128;
    const unsigned w = 128;
    const unsigned ch = 2;
    const unsigned img_size = h * w * ch;
    int8_t image_buffer[img_size] ALIGNED_4 = { 0 };

    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_YUV2,
    };
    image_cfg_t image = {
        .height = h,
        .width = w,
        .channels = ch,
        .size = h * w * ch,
        .ptr = &image_buffer[0],
        .config = &config,
    };

    // set coords and send to ISP
    camera_isp_coordinates_compute(&image);

    unsigned t0 = get_reference_time();
    for (unsigned i = 0; i < 10; i++) {
        printf("Capture %u \n", i);
        camera_isp_start_capture(c_cam, &image);
        camera_isp_get_capture(c_cam);
        delay_milliseconds_cpp(20);
    }
    unsigned t1 = get_reference_time();
    printf("Capture total time: %u [ticks]\n", (t1 - t0));

    // Save the image to file
    save_image(&image, FILE_NAME);
    exit(0);
}
