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

#include "unity_fixture.h"

#include "_helpers.h"
#include "isp_pipeline.h"            // gamma
#include "camera_utils.h"           // time


// get_reference_time();
#include <xcore/hwtimer.h>
#define TO_MS 1E-5f  

unsigned t1 = 0, t2 = 0, t3 = 0, t4 = 0, t5 = 0;
unsigned t1t = 0, t2t = 0, t3t = 0, t4t = 0, t5t = 0;


// Unity
TEST_GROUP_RUNNER(resize_group) {
  RUN_TEST_CASE(resize_group, resize__slow);
}
TEST_GROUP(resize_group);
TEST_SETUP(resize_group) { fflush(stdout); print_separator("resize_group");}
TEST_TEAR_DOWN(resize_group) {}

typedef struct  {
  float x1, y1, x2, y2, score;
} bbox_t;


static void xmodf(float a, int *b, float *c, int *bp)
{
  // split integer and decimal part
  *b = (int)(a);
  *c = a - *b;
  // last operand for convinience 
  *bp = *b + 1;
}

static unsigned img_bilinear_resize(
    bbox_t *bbox,
    const uint8_t *img,
    const uint16_t out_width,
    const uint16_t out_height,
    uint8_t *out_img)
{
    // Note : if downsample xw and yw are always < 1
    uint32_t start = get_reference_time();
    unsigned in_width = (unsigned)bbox[0].x2 - (unsigned)bbox[0].x1;
    unsigned in_height = (unsigned)bbox[0].y2 - (unsigned)bbox[0].y1;
    const float x_ratio = ((in_width - 1) / (float)(out_width - 1));
    const float y_ratio = ((in_height - 1) / (float)(out_height - 1));

    int x_l, y_l, x_h, y_h;
    float xw, yw;
    uint8_t a, b, c, d;

    for (uint16_t i = 0; i < out_height; i++)
    {
        for (uint16_t j = 0; j < out_width; j++)
        {
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

            for (uint16_t plane = 0; plane < 3; plane++)
            {
                t3 = get_reference_time();
                a = img[3 * in_width * y_l + 3 * x_l + plane];
                b = img[3 * in_width * y_l + 3 * x_h + plane];
                c = img[3 * in_width * y_h + 3 * x_l + plane];
                d = img[3 * in_width * y_h + 3 * x_h + plane];
                t3 = get_reference_time() - t3;
                t3t += t3;
                
                t4 = get_reference_time();
                printf("a: %u, b: %u, c: %u, d: %u\n", a, b, c, d);
                printf("xw: %f, yw: %f\n", xw, yw);
                uint8_t pixel = (uint8_t)(a * (1 - xw) * (1 - yw) +
                                          b * xw * (1 - yw) +
                                          c * yw * (1 - xw) +
                                          d * xw * yw);
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
    return end - start;
}

TEST(resize_group, resize__slow)
{

    // create a uint8 image
    const uint16_t channels = 3;
    const uint16_t width = 96;
    const uint16_t height = 96;
    const uint16_t out_width = 64;
    const uint16_t out_height = 64;
    bbox_t bbox = {0, 0, width-1, height-1, 0};

    uint8_t img_data[height][width][channels] = {{{0}}};
    uint8_t img_data_out[out_height][out_width][channels] = {{{0}}};
    uint8_t *img_data_ptr = (uint8_t *)&img_data[0][0][0];
    uint8_t *img_data_out_ptr = (uint8_t *)&img_data_out[0][0][0];

    // Seed the random number generator with the current time
    srand(time(NULL));

    // generate random numbers for the image buffer
    const size_t buffsize = height * width * channels;
    printf("random\n");
    fill_array_rand_uint8(img_data_ptr, buffsize);

    // resize
    printf("resize\n");    
    unsigned val = img_bilinear_resize(
        &bbox,
        img_data_ptr, 
        out_width, 
        out_height, 
        img_data_out_ptr);
    printf("total: %u\n", val);

    // Print 20 values of the output image
    printf("output\n");
    for (int i = 0; i < 20; i++)
    {
        printf("%u,", img_data_out_ptr[i]);
    }
}
