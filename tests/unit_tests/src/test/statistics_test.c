// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <timer.h>

#include "unity_fixture.h"

#include "_helpers.h"
#include "stats.h"      
#include "camera_utils.h"   // time

#define DELTA 16

// Unity
TEST_GROUP(stats_test);
TEST_SETUP(stats_test) { fflush(stdout); print_separator("stats_test");}
TEST_TEAR_DOWN(stats_test) {}
TEST_GROUP_RUNNER(stats_test) {
  RUN_TEST_CASE(stats_test, stats_test__basic);
  RUN_TEST_CASE(stats_test, stats_test__constants);
}

static void manual_mean(
  const unsigned width,
  int8_t pix_out[3][width],
  float *red, 
  float *green, 
  float *blue)
{
  for (uint32_t col=0; col < width; col++){
    *red   += pix_out[0][col] + 128.0;
    *blue  += pix_out[1][col] + 128.0;
    *green += pix_out[2][col] + 128.0;
  }
}

TEST(stats_test, stats_test__basic){
  // create a random array
  const size_t height = APP_IMAGE_HEIGHT_PIXELS / K;
  const size_t width = APP_IMAGE_WIDTH_PIXELS / K;
  const size_t channels = APP_IMAGE_CHANNEL_COUNT;
  const size_t buffsize = height * width * channels;

  // create empty stats and hist
  static statistics_t stats;
  static histograms_t histograms;

  // empty stats
  stats_reset(&histograms, &stats);

  // set seed
  srand(time(NULL));

  // compute histogram
  float tmp_red = 0.0; float tmp_green = 0.0; float tmp_blue = 0.0;
  
  for (uint32_t row=0; row < height; row++){
    // Set the array
    int8_t pix_out[3][width];
    fill_array_rand_int8((int8_t *) &pix_out[0][0], width*3);

    // histogram version
    stats_compute_histograms(&histograms, width,pix_out);

    // manual version
    manual_mean(width, pix_out, &tmp_red, &tmp_green, &tmp_blue);
  }
  tmp_red /= height; tmp_green /= height; tmp_blue /= height;

  // compute stats
  stats_compute_stats(&stats, &histograms, 1.0/buffsize);

  // print mean value
  printf("mean: %f\n", stats.stats_red.mean);
  printf("mean (slow): %f\n", tmp_red);

  TEST_ASSERT_UINT8_WITHIN (DELTA, tmp_red, stats.stats_red.mean);
}




TEST(stats_test, stats_test__constants){
  // create a random array
  const size_t height = APP_IMAGE_HEIGHT_PIXELS / K;
  const size_t width = APP_IMAGE_WIDTH_PIXELS / K;
  const size_t channels = APP_IMAGE_CHANNEL_COUNT;
  const size_t buffsize = height * width * channels;

  
  // create empty stats and hist
  static statistics_t stats;
  static histograms_t histograms;

  // empty stats
  stats_reset(&histograms, &stats);
  
  // Loop over rows
  float red = 0.0; float green = 0.0; float blue = 0.0;
  for (uint32_t row=0; row < height; row++){

    // Set the array
    int8_t pix_out[3][width];
    memset(pix_out, 100, sizeof(pix_out));

    // histogram version
    stats_compute_histograms(&histograms, width, pix_out);

    // manual version
    manual_mean(width, pix_out, &red, &green, &blue);
  }
  red /= (width*height); green /= (width*height); blue /= (width*height);
  
  // compute stats
  stats_compute_stats(&stats, &histograms, 1.0/buffsize);

  // print mean value
  printf("mean: %f\n", stats.stats_red.mean);
  printf("mean (slow): %f\n", red);

  TEST_ASSERT_UINT8_WITHIN (DELTA, red, stats.stats_red.mean);
}
