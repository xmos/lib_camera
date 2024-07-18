// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera_conv.h"
#include "_helpers.h"

// Unity
TEST_GROUP_RUNNER(timings) {
  printf("\n\nTimings table (approximate):\n");
  printf("| Cycles per Byte | Cycles per pixel | API name\n");
  RUN_TEST_CASE(timings, timing__yuv);
  RUN_TEST_CASE(timings, timing__gs);
}
TEST_GROUP(timings);
TEST_SETUP(timings) { fflush(stdout); printf("-----------------|------------------|---------\n");}
TEST_TEAR_DOWN(timings) {}

// Tests
TEST(timings, timing__yuv)
{
  // Define number of tests
  const unsigned num_tests_timing = 10;
  const unsigned bpp = 3; // num bytes in rgb
  color_table_t ct_timing_array[num_tests_timing];
  fill_color_table_uint8(&ct_timing_array[0], num_tests_timing, RGB_TO_YUV);
    
  // VPU conversion
  unsigned start = measure_time();
  for(size_t i = 0; i < num_tests_timing; i++)
  {
    camera_rgb_to_yuv(
      ct_timing_array[i].R, 
      ct_timing_array[i].G, 
      ct_timing_array[i].B);
  }
  unsigned vpu_conv_time = measure_time() - start;

  // Non VPU conversion
  start = measure_time();
  for (size_t i = 0; i < num_tests_timing; i++)
  {
    rgbToYuv(
      ct_timing_array[i].R, 
      ct_timing_array[i].G, 
      ct_timing_array[i].B);
  }
  unsigned non_vpu_conv_time = measure_time() - start;

  // Compare the time
  static const char func_name[] = "camera_rgb_to_yuv";
  PRINT_TIME(func_name, vpu_conv_time, num_tests_timing, bpp);

  static const char func_name2[] = "naive float";
  PRINT_TIME(func_name2, non_vpu_conv_time, num_tests_timing, bpp);
}

TEST(timings, timing__gs)
{
  const unsigned num_pix = 64;
  const unsigned bpp = 3; // num bytes in rgb
  __attribute__((aligned(4)))
  int8_t img[num_pix * 3];
  int8_t gs_img[num_pix];

  unsigned start = measure_time();
  camera_rgb_to_greyscale4(gs_img, img, num_pix);
  unsigned vpu4_time = measure_time() - start;

  start = measure_time();
  camera_rgb_to_greyscale16(gs_img, img, num_pix);
  unsigned vpu16_time = measure_time() - start;

  start = measure_time();
  rgb_to_greyscale_float(gs_img, img, num_pix);
  unsigned non_vpu_time = measure_time() - start;

  // Compare the time
  static const char func_name[] = "camera_rgb_to_greyscale4";
  PRINT_TIME(func_name, vpu4_time, num_pix, bpp);

  static const char func_name2[] = "camera_rgb_to_greyscale16";
  PRINT_TIME(func_name2, vpu16_time, num_pix, bpp);

  static const char func_name3[] = "naive float";
  PRINT_TIME(func_name3, non_vpu_time, num_pix, bpp);
}
