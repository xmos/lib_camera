// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <print.h>

#include "camera.h"
#include "camera_io.h"
#include "camera_isp.h"
#include "camera_conv.h"

#define in_width 64
#define in_height 64
#define in_channels 3
#define FILE_UINT8_IN     "../imgs/input_rgb_uint8_64_64.bin"
#define FILE_INT8_IN     "../imgs/input_rgb_int8_64_64.bin"

#define out_width 64
#define out_height 64
#define out_channels 3
#define FILE_UINT8_OUT    "../imgs/output_rgb_uint8_64_64.bin"
#define FILE_INT8_OUT    "../imgs/output_rgb_int8_64_64.bin"

#define FILE_LIST \
    "../imgs/input_rgb_uint8_64_64.bin", \
    "../imgs/input_rgb_int8_64_64.bin", \
    "../imgs/output_rgb_uint8_64_64.bin", \
    "../imgs/output_rgb_int8_64_64.bin"

void app() {

    // allocate space for the image
    uint8_t img_in[in_width * in_height * in_channels];
    uint8_t img_out[out_width * out_height * out_channels];
    unsigned img_size = in_width * in_height * in_channels;

    char* file_list[] = {
        FILE_LIST
    };
    uint32_t file_list_size = sizeof(file_list) / sizeof(file_list[0]);
    for (int i = 0; i < file_list_size / 2; i++)
    {

        char * file_in = file_list[i];
        char * file_out = file_list[file_list_size / 2 + i];

        printf("Reading image from file: %s\n", file_in);
        camera_io_fopen(file_in);
        camera_io_fread(img_in, img_size);

        printstrln("Rotating image");
        unsigned t0 = get_reference_time();
        camera_rotate90(img_out, img_in, in_height, in_width, in_channels);
        unsigned t1 = get_reference_time();
        printf("Time: %d\n", t1 - t0);

        printf("Writing image to file: %s\n", file_out);

        camera_io_write_image_file(file_out, img_out, out_height, out_width, out_channels);

        printstrln("Done");
    }
    camera_io_exit();
    exit(0);
}
