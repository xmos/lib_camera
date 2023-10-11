// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include <xcore/assert.h>

#include "unity_fixture.h"

#include "_helpers.h"
#include "isp_pipeline.h"            // gamma
#include "camera_utils.h"           // time

// Unity
TEST_GROUP(gamma_timing);
TEST_SETUP(gamma_timing) { fflush(stdout); print_separator("gamma_timing");}
TEST_TEAR_DOWN(gamma_timing) {}
TEST_GROUP_RUNNER(gamma_timing) {
  RUN_TEST_CASE(gamma_timing, gamma__basic);
}

static
void isp_gamma(
  int8_t * img_in,
  const int8_t *gamma_curve,
  const size_t height, 
  const size_t width, 
  const size_t channels)
{
  xassert((gamma_curve[255] != 0) && "Gamma curve is not filled correctly"); // ensure all values are filles up
  size_t buffsize = height * width * channels;
  for(size_t idx = 0; idx < buffsize; idx++){
      img_in[idx] = gamma_curve[img_in[idx]];
  }
}

// Tests
void test_gamma_size(
  const char* func_name, 
  size_t height, 
  size_t width,
  size_t channels)
{
  int8_t image_buffer[channels][height][width];

  // Seed the random number generator with the current time
  srand(time(NULL));

  // generate random numbers for the image buffer
  size_t buffsize = height * width * channels;
  fill_array_rand_int8((int8_t *) &image_buffer[0][0][0], buffsize);
  
  // then measure and apply gamma
  unsigned ts = measure_time();
  isp_gamma((int8_t *) &image_buffer[0][0][0], gamma_int8, height, width, channels);
  unsigned tdiff = measure_time() - ts;

  // print info
  printf("\tbuffsize: %d\n", buffsize);
  PRINT_NAME_TIME(func_name, tdiff);
}


TEST(gamma_timing, gamma__basic) 
{
  static const char func_name[] = "gamma downsampled";
  const size_t height = APP_IMAGE_HEIGHT_PIXELS / K;
  const size_t width = APP_IMAGE_WIDTH_PIXELS / K;
  const size_t channels = APP_IMAGE_CHANNEL_COUNT;
  test_gamma_size(func_name, height, width, channels);
}
