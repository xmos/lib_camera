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

#define DELTA_PIXEL 1 // allowed rounding error

unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
unsigned t1t = 0, t2t = 0, t3t = 0, t4t = 0, t5t = 0;


// --------------------------- Unity -----------------------------------------

TEST_GROUP_RUNNER(resize_group) {
    RUN_TEST_CASE(resize_group, resize__constant);     // resize with a constant value produces a cosntant value
    RUN_TEST_CASE(resize_group, resize__compare);      // compare the output of the opt and no opt, compare time
    RUN_TEST_CASE(resize_group, resize__upsample);     // test upsample a real image, save and decode
    RUN_TEST_CASE(resize_group, resize__int8);         // test int8 resize, constant and time
}
TEST_GROUP(resize_group);
TEST_SETUP(resize_group) {
    fflush(stdout);
    print_separator("resize_group");
}
TEST_TEAR_DOWN(resize_group) {}


// --------------------------- Image creation -----------------------------------------
typedef struct
{
    float x1, y1, x2, y2, score;
} bbox_t;

#define CREATE_IMG_UINT8(name, h, w, c)      \
struct name {                                \
    uint8_t data[h][w][c];                   \
    uint8_t* ptr;                            \
    const uint16_t width;                    \
    const uint16_t height;                   \
    const uint16_t channels;                 \
    const unsigned size;                     \
} name = {                                   \
    .ptr = (uint8_t*)&name.data[0][0][0],    \
    .width = w,                              \
    .height = h,                             \
    .channels = c,                           \
    .size = w * h * c                        \
}                             

#define CREATE_IMG_INT8(name, h, w, c)       \
struct name {                                \
    int8_t data[h][w][c];                    \
    int8_t* ptr;                             \
    const uint16_t width;                    \
    const uint16_t height;                   \
    const uint16_t channels;                 \
    const unsigned size;                     \
} name = {                                   \
    .ptr = (int8_t*)&name.data[0][0][0],     \
    .width = w,                              \
    .height = h,                             \
    .channels = c,                           \
    .size = w * h * c                        \
} 


// --------------------------- Aux functions -----------------------------------------
static void xmodf(float a, unsigned* b, float* c, unsigned* bp)
{
    // split unsignedeger and decimal part
    *b = (unsigned)(a);
    *c = a - *b;
    // last operand for convinience 
    *bp = *b + 1;
}
static void isp_resize_uint8_base(
  const uint8_t* img,
  const unsigned in_width,
  const unsigned in_height,
  uint8_t* out_img,
  const unsigned out_width,
  const unsigned out_height) {
  const float x_ratio = ((in_width - 1) / (float)(out_width - 1));
  const float y_ratio = ((in_height - 1) / (float)(out_height - 1));

  unsigned x_l, y_l, x_h, y_h;
  float xw, yw;
  uint8_t a, b, c, d;

  for (unsigned i = 0; i < out_height; i++) {
    for (unsigned j = 0; j < out_width; j++) {
      float incrx = (x_ratio * j);
      float incry = (y_ratio * i);

      xmodf(incrx, &x_l, &xw, &x_h);
      xmodf(incry, &y_l, &yw, &y_h);

      for (unsigned plane = 0; plane < 3; plane++) {
        a = img[3 * in_width * y_l + 3 * x_l + plane];
        b = img[3 * in_width * y_l + 3 * x_h + plane];
        c = img[3 * in_width * y_h + 3 * x_l + plane];
        d = img[3 * in_width * y_h + 3 * x_h + plane];

        uint8_t pixel = (uint8_t)(a * (1 - xw) * (1 - yw) +
          b * xw * (1 - yw) +
          c * yw * (1 - xw) +
          d * xw * yw);

        out_img[3 * out_width * i + 3 * j + plane] = pixel;
      }
    }
  }
}

// ------------------------------- Tests -------------------------------------

TEST(resize_group, resize__constant) {
    // create images
    CREATE_IMG_UINT8(img, 64, 64, 3);
    CREATE_IMG_UINT8(img_out, 80, 80, 3);

    // take N random pixel value
    const unsigned N_times_pixel = 100; // Number of times to fill the image with a random value
    const unsigned N_times = 100; // Number of times to pic random locations to check
    for (unsigned i = 0; i < N_times_pixel; i++) {
        // define manual cases
        uint8_t pixel_val_in;
        if (i == 0) {
            pixel_val_in = 0;
        }
        else if (i == N_times_pixel - 1) {
            pixel_val_in = 255;
        } // random cases
        else {
            pixel_val_in = rand() % 255;
        }

        memset(img.ptr, pixel_val_in, img.size);

        // resize the image - 1 (no opt)
        isp_resize_uint8_base(
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

        // resize the image - 2 (opt)
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
}


TEST(resize_group, resize__compare) {
    // create images
    CREATE_IMG_UINT8(img, 64, 64, 3);
    CREATE_IMG_UINT8(img_out, 80, 80, 3);

    // Seed the random number generator with the current time
    srand(time(NULL));

    // generate random numbers for the image buffer
    fill_array_rand_uint8(img.ptr, img.size);

    // compare pixel values
    const unsigned N_times = 100;

    for (unsigned i = 0; i < N_times; i++) {
        unsigned loc = rand() % img_out.size;
        // no opt
        isp_resize_uint8_base(
            img.ptr,
            img.width,
            img.height,
            img_out.ptr,
            img_out.width,
            img_out.height
        );
        uint8_t pixel_val_out = img_out.ptr[loc];
        // compare with opt
        isp_resize_uint8(
            img.ptr,
            img.width,
            img.height,
            img_out.ptr,
            img_out.width,
            img_out.height
        );
        uint8_t pixel_val_out_opt = img_out.ptr[loc];
        TEST_ASSERT_EQUAL_UINT8(pixel_val_out, pixel_val_out_opt);
    }

    // ---------------- Compare time
    t1 = get_reference_time();
    isp_resize_uint8_base(
        img.ptr,
        img.width,
        img.height,
        img_out.ptr,
        img_out.width,
        img_out.height
    );
    t2 = get_reference_time();
    unsigned time_no_opt = t2 - t1;

    t3 = get_reference_time();
    isp_resize_uint8(
        img.ptr,
        img.width,
        img.height,
        img_out.ptr,
        img_out.width,
        img_out.height
    );
    t4 = get_reference_time();
    unsigned time_opt = t4 - t3;

    printf("Time resize base: %d\n", time_no_opt);
    printf("Time resize: %d\n", time_opt);
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
    fread(img.ptr, sizeof(uint8_t), img.size, file);
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
}
