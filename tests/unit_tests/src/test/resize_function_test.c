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
#include "isp_functions.h"
#include "_helpers.h" // fill random array

// get_reference_time();
#include <xcore/hwtimer.h>
#define TO_MS 1E-5f

#define DELTA_PIXEL 1 // allowed rounding error

unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
unsigned t1t = 0, t2t = 0, t3t = 0, t4t = 0, t5t = 0;


// --------------------------- Unity -----------------------------------------

TEST_GROUP_RUNNER(resize_group) {
    RUN_TEST_CASE(resize_group, resize__uint8);     // test uint8 resize, constant and time
    RUN_TEST_CASE(resize_group, resize__int8);      // test int8 resize, constant and time
    RUN_TEST_CASE(resize_group, resize__upsample);  // test upsample a real image, save and decode    
}
TEST_GROUP(resize_group);
TEST_SETUP(resize_group){
    fflush(stdout);
    print_separator("resize_group");
}
TEST_TEAR_DOWN(resize_group) {}


// ------------------------------- Tests -------------------------------------

TEST(resize_group, resize__uint8) {
    // create images
    CREATE_IMG_UINT8(img, 64, 64, 3);
    CREATE_IMG_UINT8(img_out, 80, 80, 3);

    // take N random pixel value
    const unsigned N_times_pixel = 100; // Number of times to fill the image with a random value
    const unsigned N_times = 100; // Number of times to pic random locations to check
    for (unsigned i = 0; i < N_times_pixel; i++) {
        uint8_t pixel_val_in;
        // define cases
        switch (i) {
            case 1:
                pixel_val_in = 0;
                break;
            case 2:
                pixel_val_in = UINT8_MAX;
                break;
            default:
                pixel_val_in = (rand() % 256) - 128;
                break;
        }

        memset(img.ptr, pixel_val_in, img.size);

        // resize the image
        isp_resize_uint8(
            img.ptr,
            img.width,
            img.height,
            img_out.ptr,
            img_out.width,
            img_out.height
        );
        // take N random pixel location
        for (unsigned i = 0; i < N_times; i++) {
            unsigned loc = rand() % img_out.size;
            uint8_t pixel_val_out = img_out.ptr[loc];
            TEST_ASSERT_UINT8_WITHIN(DELTA_PIXEL, pixel_val_in, pixel_val_out);
        }
    }

    // Compare time
    t1 = get_reference_time();
    isp_resize_uint8(
        img.ptr,
        img.width,
        img.height,
        img_out.ptr,
        img_out.width,
        img_out.height
    );
    t2 = get_reference_time();
    unsigned time = t2 - t1;
    printf("Time resize uint8: %d\n", time);
}

TEST(resize_group, resize__int8) {
    CREATE_IMG_INT8(img, 64, 64, 3);
    CREATE_IMG_INT8(img_out, 80, 80, 3);

    const unsigned N_times_pixel = 100; // Number of times to fill the image with a random value
    const unsigned N_times = 100; // Number of times to pic random locations to check
    for (unsigned i = 0; i < N_times_pixel; i++) {
        int8_t pixel_val_in;
        // define cases
        switch (i) {
            case 0:
                pixel_val_in = INT8_MIN;
                break;
            case 1:
                pixel_val_in = 0;
                break;
            case 2:
                pixel_val_in = INT8_MAX;
                break;
            default:
                pixel_val_in = (rand() % 256) - 128;
                break;
        }

        memset(img.ptr, pixel_val_in, img.size);

        // resize the image - 1 (no opt)
        isp_resize_int8(
            img.ptr,
            img.width,
            img.height,
            img_out.ptr,
            img_out.width,
            img_out.height
        );

        // take N random pixel location
        for (unsigned i = 0; i < N_times; i++) {
            unsigned loc = rand() % img_out.size;
            int8_t pixel_val_out = img_out.ptr[loc];
            TEST_ASSERT_INT8_WITHIN(DELTA_PIXEL, pixel_val_in, pixel_val_out);
        }
    }

    // Compare time
    t1 = get_reference_time();
    isp_resize_int8(
        img.ptr,
        img.width,
        img.height,
        img_out.ptr,
        img_out.width,
        img_out.height
    );
    t2 = get_reference_time();
    unsigned time = t2 - t1;
    printf("Time resize int8: %d\n", time);
}


TEST(resize_group, resize__upsample) {
    // create images
    CREATE_IMG_UINT8(img, 64, 64, 3);
    CREATE_IMG_UINT8(img_out, 80, 80, 3);

    // read an image
    printf("Reading image...\n");
    char* filename = "imgs/person.bin";
    char* filename_out = "imgs/person_upsampled.bin";

    FILE* file = fopen(filename, "rb");
    assert(file != NULL && "File not found");
    size_t bytes_read = fread(img.ptr, sizeof(uint8_t), img.size, file);
    assert(bytes_read == img.size);
    fclose(file);

    // upsample the image
    isp_resize_uint8(
        img.ptr,
        img.width,
        img.height,
        img_out.ptr,
        img_out.width,
        img_out.height
    );

    // save the image
    printf("Saving image...\n");
    file = fopen(filename_out, "wb");
    assert(file != NULL);
    fwrite(img_out.ptr, sizeof(uint8_t), img_out.size, file);
    fclose(file);

    // decode the image
    char cmd[200];
    sprintf(cmd, "python ../../python/decode_downsampled.py --input imgs/person_upsampled.bin --width %d --height %d", img_out.width, img_out.height);
    //system(cmd);
    printf("Run the cmd >> : %s\n", cmd);
}
