// Copyright 2020-2024 XMOS LIMITED.
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


#define INV_DELTA 20  // error allowed in YUV RGB color conversion 
#define num_tests 3

// Define color conversion table and initialize random
color_table_t ct_test_vector[num_tests];

// Unity
TEST_GROUP_RUNNER(color_conversion) {
  RUN_TEST_CASE(color_conversion, conversion__yuv_to_rgb);
  RUN_TEST_CASE(color_conversion, conversion__rgb_to_yuv);
  RUN_TEST_CASE(color_conversion, conversion__yuv_timming);
  RUN_TEST_CASE(color_conversion, conversion__rgb_to_gs);
  RUN_TEST_CASE(color_conversion, conversion__gs_timing);
}
TEST_GROUP(color_conversion);
TEST_SETUP(color_conversion) { fflush(stdout); print_separator("color_conversion");}
TEST_TEAR_DOWN(color_conversion) {}

// Tests
TEST(color_conversion, conversion__yuv_to_rgb)
{
  // initialize with random values
  fill_color_table_uint8(&ct_test_vector[0], num_tests, YUV_TO_RGB);

  for(size_t i = 0; i < num_tests; i++)
  {
    // Define color table 
    color_table_t ct_ref = ct_test_vector[i];
    color_table_t ct_result = {0};

    // Test the converison
    yuv_to_rgb_ct(&ct_ref, &ct_result);

    printColorTable(&ct_ref, 1);
    printColorTable(&ct_result, 0);

    // Ensure conversion is correct
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.R, ct_result.R);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.G, ct_result.G);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.B, ct_result.B);
  }
}


TEST(color_conversion, conversion__rgb_to_yuv)
{
  // initialize with random values
  fill_color_table_uint8(&ct_test_vector[0], num_tests, RGB_TO_YUV);

  for(size_t i = 0; i < num_tests; i++)
  {
    // Define color table 
    color_table_t ct_ref = ct_test_vector[i];
    color_table_t ct_result = {0};

    // Test the converison
    rgb_to_yuv_ct(&ct_ref, &ct_result);

    printColorTable(&ct_ref, 1);
    printColorTable(&ct_result, 0);

    // Printing the extracted bytes
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.Y, ct_result.Y);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.U, ct_result.U);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.V, ct_result.V);
  }
}

TEST(color_conversion, conversion__yuv_timming)
{
  // Define number of tests
  const unsigned num_tests_timing = 10;
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
  printf("\tnumber of conversions: %d\n", num_tests_timing);
  static const char func_name[] = "YUV color conversion VPU";
  PRINT_NAME_TIME(func_name, vpu_conv_time);

  static const char func_name2[] = "YUV color conversion non VPU";
  PRINT_NAME_TIME(func_name2, non_vpu_conv_time);
}

TEST(color_conversion, conversion__rgb_to_gs)
{
  const unsigned num_pix = 32;
  __attribute__((aligned(4)))
  int8_t img[num_tests][num_pix * 3];
  int8_t gs_img4[num_pix];
  int8_t gs_img16[num_pix];
  int8_t gs_img_ref[num_pix];

  fill_array_rand_int8((int8_t *)img, num_tests * num_pix * 3);

  for(unsigned i = 0; i < num_tests; i++) {
    camera_rgb_to_greyscale4(gs_img4, &img[i][0], num_pix);
    camera_rgb_to_greyscale16(gs_img16, &img[i][0], num_pix);
    rgb_to_greyscale_float(gs_img_ref, &img[i][0], num_pix);

    for(unsigned j = 0; j < num_pix; j++) {
      TEST_ASSERT_INT8_WITHIN(1, gs_img4[j], gs_img_ref[j]);
      TEST_ASSERT_INT8_WITHIN(1, gs_img16[j], gs_img_ref[j]);
    }
  }
}

TEST(color_conversion, conversion__gs_timing)
{
  const unsigned num_pix = 64;
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
  printf("\tnumber of %u-pixel conversions: %d\n", num_pix, 1);
  static const char func_name[] = "GS color conversion 4 pix VPU";
  PRINT_NAME_TIME(func_name, vpu4_time);

  static const char func_name2[] = "GS color conversion 16 pix VPU";
  PRINT_NAME_TIME(func_name2, vpu16_time);

  static const char func_name3[] = "GS color conversion non VPU";
  PRINT_NAME_TIME(func_name3, non_vpu_time);
}
