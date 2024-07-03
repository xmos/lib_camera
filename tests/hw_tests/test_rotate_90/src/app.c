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
#define FILE_IN     "../imgs/input_rgb_uint8_64_64.bin"

#define out_width 64
#define out_height 64
#define out_channels 3
#define FILE_OUT    "../imgs/output_rgb_uint8_64_64.bin"


void app() {
    
    // allocate space for the image
    uint8_t img_in[in_width * in_height * in_channels];
    uint8_t img_out[out_width * out_height * out_channels];
    unsigned img_size = in_width * in_height * in_channels;

    printstrln("Reading image from file");
    camera_io_fopen(FILE_IN);
    camera_io_fill_array_from_file(img_in, img_size);

    printstrln("Rotating image");
    unsigned t0 = get_reference_time();
    camera_rotate90_uint8(img_out, img_in, in_height, in_width, in_channels);
    unsigned t1 = get_reference_time();
    printf("Time: %d\n", t1 - t0);

    printstrln("Writing image to file");
    camera_io_write_image_file(FILE_OUT, img_out, out_height, out_width, out_channels);

    printstrln("Done");
    xscope_close_all_files();
    exit(0);
}
