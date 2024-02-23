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
#include "_helpers.h" // fill random array

// get_reference_time();
#include <xcore/hwtimer.h>
#define TO_MS 1E-5f

unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
unsigned t1t = 0, t2t = 0, t3t = 0, t4t = 0, t5t = 0;

// Unity
TEST_GROUP_RUNNER(resize_group) {
    RUN_TEST_CASE(resize_group, resize__upsample);
    RUN_TEST_CASE(resize_group, resize__compare);
    RUN_TEST_CASE(resize_group, resize__constant);
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

const uint16_t channels = 3;
const uint16_t width = 96, height = 96;
const uint16_t out_width = 64, out_height = 64;
bbox_t bbox = { 0, 0, width - 1, height - 1, 0 };

uint8_t img_data[height][width][channels] = { {{0}} };
uint8_t img_data_out[out_height][out_width][channels] = { {{0}} };
uint8_t* img_data_ptr = (uint8_t*)&img_data[0][0][0];
uint8_t* img_data_out_ptr = (uint8_t*)&img_data_out[0][0][0];

const unsigned in_size = height * width * channels;
const unsigned out_size = out_height * out_width * channels;
// --------------------------------------------------------------------

static void xmodf(float a, int* b, float* c, int* bp) {
    // split integer and decimal part
    *b = (int)(a);
    *c = a - *b;
    // last operand for convinience
    *bp = *b + 1;
}

static void img_bilinear_resize(
    bbox_t* bbox,
    const uint8_t* img,
    const uint16_t out_width,
    const uint16_t out_height,
    uint8_t* out_img) {
    // Note : if downsample xw and yw are always < 1
    uint32_t start = get_reference_time();
    unsigned in_width = (unsigned)bbox[0].x2 - (unsigned)bbox[0].x1;
    unsigned in_height = (unsigned)bbox[0].y2 - (unsigned)bbox[0].y1;
    const float x_ratio = ((in_width - 1) / (float)(out_width - 1));
    const float y_ratio = ((in_height - 1) / (float)(out_height - 1));

    int x_l, y_l, x_h, y_h;
    float xw, yw;
    uint8_t a, b, c, d;

    for (uint16_t i = 0; i < out_height; i++) {
        for (uint16_t j = 0; j < out_width; j++) {
            t1 = get_reference_time();
            float incrx = (x_ratio * j);
            float incry = (y_ratio * i);
            t1 = get_reference_time() - t1;
            t1t += t1;

            t2 = get_reference_time();
            xmodf(incrx, &x_l, &xw, &x_h);
            xmodf(incry, &y_l, &yw, &y_h);
            t2 = get_reference_time() - t2;
            t2t += t2;

            float xw_inv = (1 - xw);
            float yw_inv = (1 - yw);

            float W = xw_inv * yw_inv;
            float X = xw * yw_inv;
            float Y = yw * xw_inv;
            float Z = xw * yw;

            for (uint16_t plane = 0; plane < 3; plane++) {
                t3 = get_reference_time();
                a = img[3 * in_width * y_l + 3 * x_l + plane];
                b = img[3 * in_width * y_l + 3 * x_h + plane];
                c = img[3 * in_width * y_h + 3 * x_l + plane];
                d = img[3 * in_width * y_h + 3 * x_h + plane];
                t3 = get_reference_time() - t3;
                t3t += t3;

                t4 = get_reference_time();
                uint8_t pixel = (uint8_t)(a * W + b * X + c * Y + d * Z);
                t4 = get_reference_time() - t4;
                t4t += t4;

                t5 = get_reference_time();
                out_img[3 * out_width * i + 3 * j + plane] = pixel;
                t5 = get_reference_time() - t5;
                t5t += t5;
            }
        }
    }

    // print all times
    printf("t1: %u\n", t1t);
    printf("t2: %u\n", t2t);
    printf("t3: %u\n", t3t);
    printf("t4: %u\n", t4t);
    printf("t5: %u\n", t5t);

    uint32_t end = get_reference_time();
    printf("total: %lu\n", end - start);
}

static inline float uint8_to_float(const uint8_t val) {
    int32_t mant = val, exp = 23, zero = 0;
    float res;
    asm("fmake %0, %1, %2, %3, %4" : "=r"(res) : "r"(zero), "r"(exp), "r"(zero), "r"(mant));
    return res;
}

static inline uint8_t float_to_uint8(const float val) {
    // this assumes that the input [0.0, 255.0]
    int32_t zero, exp, mant;
    asm("fsexp %0, %1, %2" : "=r"(zero), "=r"(exp) : "r"(val));
    asm("fmant %0, %1" : "=r"(mant) : "r"(val));
    exp -= 23; // because
    mant >>= -exp;
    return (uint8_t)(mant & 0xff);
}

static inline float unsigned_to_float(const unsigned val) {
    // will lose some data with long words
    int32_t exp = 23, zero = 0;
    float res;
    asm("fmake %0, %1, %2, %3, %4" : "=r"(res) : "r"(zero), "r"(exp), "r"(zero), "r"(val));
    return res;
}

static inline unsigned float_to_unsigned(const float val) {
    int32_t zero, exp, mant;
    asm("fsexp %0, %1, %2" : "=r"(zero), "=r"(exp) : "r"(val));
    asm("fmant %0, %1" : "=r"(mant) : "r"(val));
    exp -= 23; // because
    mant >>= -exp;
    // saturate
    if (exp > 8) {
        mant = UINT32_MAX;
    }
    return mant;
}

static void img_bilinear_resize_opt(
    bbox_t* bbox,
    const uint8_t* img,
    const uint16_t out_width,
    const uint16_t out_height,
    uint8_t* out_img) {
    // Note : if downsample xw and yw are always < 1
    uint32_t start = get_reference_time();
    unsigned in_width = float_to_unsigned(bbox[0].x2) - float_to_unsigned(bbox[0].x1);
    unsigned in_height = float_to_unsigned(bbox[0].y2) - float_to_unsigned(bbox[0].y1);
    const float x_ratio = (unsigned_to_float(in_width - 1) / unsigned_to_float(out_width - 1));
    const float y_ratio = (unsigned_to_float(in_height - 1) / unsigned_to_float(out_height - 1));

    int x_l, y_l, x_h, y_h;
    float xw, yw;
    uint8_t a, b, c, d;

    for (unsigned i = 0; i < out_height; i++) {
        float incry = (y_ratio * unsigned_to_float(i));
        xmodf(incry, &y_l, &yw, &y_h);
        float yw_inv = (1 - yw);

        for (unsigned j = 0; j < out_width; j++) {
            t1 = get_reference_time();
            float incrx = (x_ratio * unsigned_to_float(j));
            t1 = get_reference_time() - t1;
            t1t += t1;

            t2 = get_reference_time();
            xmodf(incrx, &x_l, &xw, &x_h);
            t2 = get_reference_time() - t2;
            t2t += t2;

            float xw_inv = (1 - xw);

            float W = xw_inv * yw_inv;
            float X = xw * yw_inv;
            float Y = yw * xw_inv;
            float Z = xw * yw;

            for (unsigned plane = 0; plane < 3; plane++) {
                t3 = get_reference_time();
                a = img[3 * in_width * y_l + 3 * x_l + plane];
                b = img[3 * in_width * y_l + 3 * x_h + plane];
                c = img[3 * in_width * y_h + 3 * x_l + plane];
                d = img[3 * in_width * y_h + 3 * x_h + plane];
                t3 = get_reference_time() - t3;
                t3t += t3;

                t4 = get_reference_time();
                // can be optimised with fmacc
                uint8_t pixel = float_to_uint8(uint8_to_float(a) * W + uint8_to_float(b) * X + uint8_to_float(c) * Y + uint8_to_float(d) * Z);

                t4 = get_reference_time() - t4;
                t4t += t4;

                t5 = get_reference_time();
                out_img[3 * out_width * i + 3 * j + plane] = pixel;
                t5 = get_reference_time() - t5;
                t5t += t5;
            }
        }
    }

    // print all times
    printf("t1: %u\n", t1t);
    printf("t2: %u\n", t2t);
    printf("t3: %u\n", t3t);
    printf("t4: %u\n", t4t);
    printf("t5: %u\n", t5t);

    uint32_t end = get_reference_time();
    printf("total: %lu\n", end - start);
}

TEST(resize_group, resize__compare) {
    // Seed the random number generator with the current time
    srand(time(NULL));

    // generate random numbers for the image buffer
    fill_array_rand_uint8(img_data_ptr, in_size);

    // compare pixel values
    const unsigned N_times = 100;

    for (unsigned i = 0; i < N_times; i++) {
        unsigned loc = rand() % out_size;

        img_bilinear_resize(
            &bbox,
            img_data_ptr,
            out_width,
            out_height,
            img_data_out_ptr);
        uint8_t pixel_val_out = img_data_out_ptr[loc];
        img_bilinear_resize_opt(
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

        img_bilinear_resize_opt(
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

typedef struct {
    uint8_t* ptr;
    const uint16_t width;
    const uint16_t height;
    const uint16_t channels;
    const size_t size;
} Image_t;

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
    bbox_t bbox = { 0, 0, img_st.width, img_st.height, 0 };
    img_bilinear_resize(
        &bbox,
        img_st.ptr,
        img_out_st.width,
        img_out_st.height,
        img_out_st.ptr);

    // save the image
    printf("Saving image...\n");
    file = fopen(filename_out, "wb");
    assert(file != NULL);
    fwrite(img_out_st.ptr, sizeof(uint8_t), img_out_st.size, file);
    fclose(file);

    // decode the image
    char cmd[200];
    sprintf(cmd, "python ../../python/decode_downsampled.py --input imgs/person_upsampled.bin --width %d --height %d", img_out_st.width, img_out_st.height);
    system(cmd);
}
