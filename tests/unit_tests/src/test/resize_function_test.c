// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <print.h>

#include <xcore/assert.h>

#include "camera_io_utils.h"
#include "unity_fixture.h"
#include "isp_functions.hpp"
#include "_helpers.h" // fill random array

// get_reference_time();
#include <xcore/hwtimer.h>
#define TO_MS 1E-5f

unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
unsigned t1t = 0, t2t = 0, t3t = 0, t4t = 0, t5t = 0;

// Unity
TEST_GROUP_RUNNER(resize_group) {
    RUN_TEST_CASE(resize_group, resize__upsample);
    //RUN_TEST_CASE(resize_group, resize__compare);
    //RUN_TEST_CASE(resize_group, resize__constant);
}
TEST_GROUP(resize_group);
TEST_SETUP(resize_group) {
    fflush(stdout);
    print_separator("resize_group");
}
TEST_TEAR_DOWN(resize_group) {}


// --------------------------------------------------------------------
// Image creation
typedef struct
{
    float x1, y1, x2, y2, score;
} bbox_t;

typedef struct {
    uint8_t* ptr;
    const uint16_t width;
    const uint16_t height;
    const uint16_t channels;
    const size_t size;
} Image_t;

// --------------------------------------------------------------------

TEST(resize_group, resize__upsample) {
    // image in
    const unsigned width = 64, height = 64, channels = 3;
    uint8_t img[height][width][channels];
    Image_t img_st = { (uint8_t*)&img[0][0][0], width, height, channels, width * height * channels };

    // image out
    const unsigned out_width = 80, out_height = 80, out_channels = 3;
    uint8_t img_out[out_height][out_width][out_channels];
    Image_t img_out_st = { (uint8_t*)&img_out[0][0][0], out_width, out_height, out_channels, out_width * out_height * out_channels };

    // read an image
    printf("Reading image...\n");
    char* filename = "imgs/person.bin";
    char* filename_out = "imgs/person_upsampled.bin";

    FILE* file = fopen(filename, "rb");
    assert(file != NULL);
    fread(img_st.ptr, sizeof(uint8_t), img_st.size, file);
    fclose(file);

    // upsample the image
    printf("Upsampling image...\n");
    unsigned t1 = get_reference_time();
    isp_resize_uint8_opt(
        img_st.ptr,
        img_st.width,
        img_st.height,
        img_out_st.ptr,
        img_out_st.width,
        img_out_st.height
    );
    unsigned t2 = get_reference_time();
    printf("Time: %u\n", t2 - t1);
    // save the image
    printf("Saving image...\n");
    file = fopen(filename_out, "wb");
    assert(file != NULL);
    fwrite(img_out_st.ptr, sizeof(uint8_t), img_out_st.size, file);
    fclose(file);

    // decode the image
    char cmd[200];
    sprintf(cmd, "python ../../python/decode_downsampled.py --input imgs/person_upsampled.bin --width %d --height %d", img_out_st.width, img_out_st.height);
    //system(cmd);
    printf("Run the cmd >> : %s\n", cmd);
}

/*
TEST(resize_group, resize__compare) {
    // Seed the random number generator with the current time
    srand(time(NULL));

    // generate random numbers for the image buffer
    fill_array_rand_uint8(img_data_ptr, in_size);

    // compare pixel values
    const unsigned N_times = 100;

    for (unsigned i = 0; i < N_times; i++) {
        unsigned loc = rand() % out_size;

        isp_resize_uint8(
            &bbox,
            img_data_ptr,
            out_width,
            out_height,
            img_data_out_ptr);
        uint8_t pixel_val_out = img_data_out_ptr[loc];
        isp_resize_uint8_opt(
            &bbox,
            img_data_ptr,
            out_width,
            out_height,
            img_data_out_ptr);
        uint8_t pixel_val_out_opt = img_data_out_ptr[loc];

        TEST_ASSERT_EQUAL_UINT8(pixel_val_out, pixel_val_out_opt);
    }
}

TEST(resize_group, resize__constant) {
    // allowed rounding error
    const unsigned delta = 1;

    // take N random pixel value
    const unsigned N_times_pixel = 100;
    const unsigned N_times = 100;
    for (unsigned i = 0; i < N_times_pixel; i++) {
        // define manual cases
        uint8_t pixel_val_in;
        if (i == 0) {
            pixel_val_in = 0;
        }
        else if (i == N_times_pixel - 1) {
            pixel_val_in = 255;
        }
        else {
            pixel_val_in = rand() % 255;
        }

        memset(img_data, pixel_val_in, in_size);

        isp_resize_uint8_opt(
            &bbox,
            img_data_ptr,
            out_width,
            out_height,
            img_data_out_ptr);

        // take N random pixel location
        for (unsigned i = 0; i < N_times; i++) {
            unsigned loc = rand() % out_size;
            uint8_t pixel_val_out = img_data_out_ptr[loc];
            TEST_ASSERT_UINT8_WITHIN(delta, pixel_val_in, pixel_val_out);
        }
    }
}
*/
