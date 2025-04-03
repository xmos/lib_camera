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

// Unity
TEST_GROUP_RUNNER(white_balance) {
    RUN_TEST_CASE(white_balance, white_balance__simple);
    //TODO add timings unit test
    //TODO add test for different sizes
}
TEST_GROUP(white_balance);
TEST_SETUP(white_balance) { fflush(stdout); print_separator("white_balance");}
TEST_TEAR_DOWN(white_balance) {}


// Tests
TEST(white_balance, white_balance__simple)
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
