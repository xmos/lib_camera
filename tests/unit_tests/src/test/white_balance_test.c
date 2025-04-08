// Copyright 2020-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera_isp.h"
#include "helpers.h"

#define WB_gain_RED    1.538
#define WB_gain_GREEN  1.0
#define WB_gain_BLUE   1.587
#define DELTA 5

// Unity
TEST_GROUP_RUNNER(white_balance) {
    RUN_TEST_CASE(white_balance, white_balance__simple);
    RUN_TEST_CASE(white_balance, white_balance__odd);
    //TODO add timings unit test
    //TODO add test for different sizes
}
TEST_GROUP(white_balance);
TEST_SETUP(white_balance) { fflush(stdout); print_separator("white_balance");}
TEST_TEAR_DOWN(white_balance) {}


// C implementation
static inline
void plain_wb(int8_t* arr, unsigned arr_size){
    float pfilters[3] = {
        WB_gain_RED,
        WB_gain_GREEN,
        WB_gain_BLUE
    };
    unsigned pos = 0;
    for (unsigned i = 0; i < arr_size; i++)
    {
        float tmp = pfilters[pos] * ((float)arr[i] + 128) - 127;
        tmp = tmp > INT8_MAX ? INT8_MAX : tmp;
        tmp = tmp < INT8_MIN ? INT8_MIN : tmp;
        arr[i] = (int8_t) tmp;
        pos = (pos + 1) % 3;
    }
}


// Tests
TEST(white_balance, white_balance__simple) // ensure we dont write zeros after the img
{
    const unsigned height = 1;
    const unsigned width = 13;
    const unsigned channels = 3;
    const unsigned zeros = 5;
    const unsigned size = height * width * channels;
    const unsigned size_with_zeros = size + zeros;
    int8_t buff[size_with_zeros] ALIGNED_4 = { 0 };    
    image_cfg_t image = {
		.height = height,
		.width = width,
		.channels = 3,
		.size = size,
		.ptr = buff,
		.config = NULL
	};
    fill_array_rand_int8(buff, size);
    camera_isp_white_balance(&image);

    // print output image
    print_array(buff, size_with_zeros);

    // assert last N are zeros
    for (unsigned i = size; i < size_with_zeros; i++) {
        TEST_ASSERT_EQUAL_INT8(0, buff[i]);
    }
}


TEST(white_balance, white_balance__odd) // non multiple of 4 size
{
    const unsigned height = 1;
    const unsigned width = 5;
    const unsigned channels = 3;
    const unsigned size = height * width * channels;
    int8_t buff[size] ALIGNED_4 = { 0 };
    int8_t buff2[size] ALIGNED_4 = { 0 };
    image_cfg_t image = {
		.height = height,
		.width = width,
		.channels = channels,
		.size = size,
		.ptr = buff,
		.config = NULL
	};
    //fill_array_rand_int8(buff, size);
    for (unsigned i = 0; i < size; i++) {
        buff[i] = i;
    }
    
    memcpy(buff2, buff, size * sizeof(int8_t));

    // do both wb
    camera_isp_white_balance(&image);
    plain_wb(buff2, size);

    // print output image
    for (unsigned i = 0; i < size; i++) {
        printf("buff[%d] = %d, buff2[%d] = %d\n", i, buff[i], i, buff2[i]);
        TEST_ASSERT_INT8_WITHIN(10, buff[i], buff2[i]);
    }
}
