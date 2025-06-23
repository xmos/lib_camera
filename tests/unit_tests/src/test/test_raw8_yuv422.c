// Copyright 2020-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include <xcore/hwtimer.h>

#include "unity_fixture.h"

#include "kernels_yuv.h"
#include "camera_isp.h"
#include "helpers.h"

#define MARGIN 3 // margin for the test
#define CLAMP(x) ((x < INT8_MIN) ? INT8_MIN : (x > INT8_MAX) ? INT8_MAX : x)

// Unity
TEST_GROUP_RUNNER(yuv) {
    RUN_TEST_CASE(yuv, yuv__simple);
    RUN_TEST_CASE(yuv, yuv__time);
}
TEST_GROUP(yuv);
TEST_SETUP(yuv) { fflush(stdout); print_separator("yuv");}
TEST_TEAR_DOWN(yuv) {}

static
void vpu_set_int8_mode(){
    asm volatile("vclrdr");
    asm volatile("ldc r11, 0x200");
    asm volatile("vsetc r11");
}

// C implementation
static 
void raw8_block_to_yuv422_cimpl(
    int8_t *dst,
    int8_t *src,
    unsigned line_size)
{    
    // YUV (BT.601) fixed point coeffs
    // const int Y_coeff[3] = {66, 129, 25};
    // const int U_coeff[3] = {-38, -74, 112};
    // const int V_coeff[3] = {112, -94, -18};
    // Corrected YUV
    const int a = 47, b = 61, c = 18;
    const int d = -27, e = -34, f = 82;
    const int g = 80, h = -43, i = -13;

    static const int8_t yk = 0;
    static const int8_t uk = 21;
    static const int8_t vk = 23;

    const unsigned steps = 4;
    unsigned loop_size = 16;
    
    for (unsigned x = 0; x <= loop_size - 4; x += steps) {
        // Load 2 RAW pixels
        int r0 = src[0 * line_size + x + 0];
        int g0 = src[0 * line_size + x + 1];
        int r1 = src[0 * line_size + x + 2];
        int g1 = src[0 * line_size + x + 3];
        int b0 = src[1 * line_size + x + 1];
        int b1 = src[1 * line_size + x + 3];
        // MACCS
        int Y0 = (a * r0 + b * g0 + c * b0); 
        int U0 = (d * r0 + e * g0 + f * b0); 
        int V0 = (g * r0 + h * g0 + i * b0); 
        int Y1 = (a * r1 + b * g1 + c * b1);
        // SATS
        Y0 = Y0 >> 7;U0 = U0 >> 7;
        Y1 = Y1 >> 7;V0 = V0 >> 7;
        // ADDS
        Y0 += yk; U0 += uk;
        Y1 += yk; V0 += vk;
        // Convert to int8_t
        Y0 = CLAMP(Y0); U0 = CLAMP(U0);
        Y1 = CLAMP(Y1); V0 = CLAMP(V0);
        // Output
        dst[x+0] = Y0;
        dst[x+1] = U0;
        dst[x+2] = Y1;
        dst[x+3] = V0;
    }
}

// VPU implementation
extern void raw8_block_to_yuv422(
    int8_t *dst,
    int8_t *src,
    unsigned line_size
);


// TEST for raw8 to yuv422 conversion
TEST(yuv, yuv__simple)
{
    // constants
    const unsigned img_width = 16;
    const unsigned n_runs = 8;

    // inputs and outputs
    int8_t input_rows[2][img_width];    
    int8_t *input_ptr = (int8_t *)input_rows;
    int8_t output_c[img_width] = { 0 };
    int8_t output_vpu[img_width] = { 0 };

    // set vpu 
    vpu_set_int8_mode();

    for (unsigned run = 0; run < n_runs; run++) {
        // fill input 
        fill_array_rand_int8(input_rows[0], img_width);
        fill_array_rand_int8(input_rows[1], img_width);

        // run conversions
        raw8_block_to_yuv422(output_vpu, input_ptr, img_width);
        raw8_block_to_yuv422_cimpl(output_c, input_ptr, img_width);

        // compare outputs
        for (unsigned i = 0; i < img_width; i++) {
            TEST_ASSERT_INT8_WITHIN(MARGIN, output_c[i], output_vpu[i]);
        }
    }
    printf("raw8 to yuv422 conversion passed for %u runs\n", n_runs);
}

// TEST for raw8 to yuv422 measure time
static
unsigned timed_raw8_block_to_yuv422(
    int8_t *dst,
    int8_t *src,
    unsigned line_size)
{
    uint32_t start = get_reference_time();
    raw8_block_to_yuv422(dst, src, line_size);
    uint32_t end = get_reference_time();
    return end - start;
}

static
unsigned timed_raw8_block_to_yuv422_cimpl(
    int8_t *dst,
    int8_t *src,
    unsigned line_size)
{
    uint32_t start = get_reference_time();
    raw8_block_to_yuv422_cimpl(dst, src, line_size);
    uint32_t end = get_reference_time();
    return end - start;
}

TEST(yuv, yuv__time)
{
    // constants
    const unsigned img_width = 16;
    const uint32_t n_runs = 8;

    // inputs and outputs
    int8_t input_rows[2][img_width];
    int8_t* input_ptr = (int8_t*)input_rows;
    int8_t output_c[img_width] = { 0 };
    int8_t output_vpu[img_width] = { 0 };

    // set vpu 
    vpu_set_int8_mode();
    uint32_t total_time_c = 0, total_time_vpu = 0;

    for (unsigned run = 0; run < n_runs; run++) {
        fill_array_rand_int8(input_rows[0], img_width);
        fill_array_rand_int8(input_rows[1], img_width);
        total_time_vpu += timed_raw8_block_to_yuv422(output_vpu, input_ptr, img_width);
        total_time_c += timed_raw8_block_to_yuv422_cimpl(output_c, input_ptr, img_width);
    }
    float avg_time_c = (float)total_time_c / n_runs;
    float avg_time_vpu = (float)total_time_vpu / n_runs;
    float ratio = avg_time_c / avg_time_vpu;
    printf("avg_time_c: %.2f [ticks/block], avg_time_vpu: %.2f [ticks/block], ratio: %.2f\n", avg_time_c, avg_time_vpu, ratio);
}
