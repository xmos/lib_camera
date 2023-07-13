// Copyright 2020-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera_main.h"
#include "camera_utils.h"

#include "cpp/image_hfilter.hpp"

extern "C" {

TEST_GROUP_RUNNER(ImageHFilter) {
  RUN_TEST_CASE(ImageHFilter, ImageHFilter_update_gain__case1);
  RUN_TEST_CASE(ImageHFilter, ImageHFilter_update_gain__case2);
  RUN_TEST_CASE(ImageHFilter, ImageHFilter_update_gain__case3);
  RUN_TEST_CASE(ImageHFilter, ImageHFilter_update_gain__case4);
  RUN_TEST_CASE(ImageHFilter, ImageHFilter_update_gain__timing);

  // RUN_TEST_CASE(ImageHFilter, ImageHFilter__case1);
}

TEST_GROUP(ImageHFilter);
TEST_SETUP(ImageHFilter) { fflush(stdout); }
TEST_TEAR_DOWN(ImageHFilter) {}




///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(ImageHFilter, ImageHFilter_update_gain__case1)
{
  const float gain = 0.0f;
  const size_t offset = 0;

  ImageHFilter hfilter(160, 4, offset, 16);

  hfilter.update_gain(gain);

  const unsigned exp_shift = 9;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = - 128 * shift_scale - 16 * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0, hfilter.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0, hfilter.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0, hfilter.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, hfilter.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, hfilter.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(ImageHFilter, ImageHFilter_update_gain__case2)
{

  const float gain = 1.0f;
  const size_t offset = 1;

  ImageHFilter hfilter(160, 4, offset, 16);

  hfilter.update_gain(gain);

  const unsigned exp_shift = 7;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = - 16 * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x1B, hfilter.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x4B, hfilter.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x1B, hfilter.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, hfilter.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, hfilter.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(ImageHFilter, ImageHFilter_update_gain__case3)
{
  const float gain = 1.2f;
  const size_t offset = 0;

  ImageHFilter hfilter(160, 4, offset, 16);

  hfilter.update_gain(gain);

  const unsigned exp_shift = 7;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * shift_scale - 16 * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x20, hfilter.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x59, hfilter.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x20, hfilter.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, hfilter.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, hfilter.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(ImageHFilter, ImageHFilter_update_gain__case4)
{
  const float gain = 0.8f;
  const size_t offset = 1;

  ImageHFilter hfilter(160, 4, offset, 16);

  hfilter.update_gain(gain);

  const unsigned exp_shift = 8;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * shift_scale - SENSOR_BLACK_LEVEL * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x2B, hfilter.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x77, hfilter.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x2B, hfilter.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, hfilter.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, hfilter.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(ImageHFilter, ImageHFilter_update_gain__timing)
{
  const float gain = 1.0f;

  ImageHFilter hfilter(160, 4, 0, 16);

  unsigned ts = measure_time();
  hfilter.update_gain(gain);
  unsigned te = measure_time();

  static const char func_name[] = "ImageHfilter.update_gain()";

  printf("\n\t%s timing: %u ticks\n\n", func_name, te - ts);
}

}