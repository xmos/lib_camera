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
#include <xcore/hwtimer.h>
#define TO_MS 1E-5f

#include "camera_io_utils.h"
#include "unity_fixture.h"
#include "isp_functions.h"
#include "_helpers.h"

// --------------------------- Unity -----------------------------------------

TEST_GROUP_RUNNER(crop_group) {
    RUN_TEST_CASE(crop_group, crop__time);
}
TEST_GROUP(crop_group);
TEST_SETUP(crop_group) {
    fflush(stdout);
    print_separator("crop_group");
}
TEST_TEAR_DOWN(crop_group) {}

// ------------------------------- Tests -------------------------------------

TEST(crop_group, crop__time){
    CREATE_IMG_UINT8(img, 64, 64, 3);

    unsigned t1 = get_reference_time();
    isp_crop_uint8(img.ptr,img.width, img.height, 10,10,50,50);
    unsigned t2 = get_reference_time();
    printf("crop__time: %d\n", t2-t1);
}
