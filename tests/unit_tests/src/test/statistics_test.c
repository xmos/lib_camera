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
#include "isp_stats.h"      
#include "camera_utils.h"   // time

#define DELTA 16

#define PRINT_HIST 0

// Test size
#define HEIGHT        APP_IMAGE_HEIGHT_PIXELS / K
#define WIDTH         APP_IMAGE_WIDTH_PIXELS / K
#define BUFFSIZE      (float)(HEIGHT * WIDTH)
#define INV_BUFFSIZE  (float)(1.0 / BUFFSIZE)

// Unity
TEST_GROUP(stats_test);
TEST_SETUP(stats_test) { fflush(stdout); print_separator("stats_test");}
TEST_TEAR_DOWN(stats_test) {}
TEST_GROUP_RUNNER(stats_test) {
  RUN_TEST_CASE(stats_test, stats_test__basic);
  RUN_TEST_CASE(stats_test, stats_test__constants);
  RUN_TEST_CASE(stats_test, stats_test__edge);
}


// Auxiliary functions
#if PRINT_HIST
  static void print_hist(channel_histogram_t *hist){
    printf("hist: [");
    for (uint32_t k=0; k < HISTOGRAM_BIN_COUNT; k++){
      printf("%ld,", hist->bins[k]);
    }
    printf("]\n");
  }
#endif

static 
void manual_mean(
  const unsigned w,
  int8_t pix_out[3][w],
  float *red, 
  float *green, 
  float *blue)
{
  for (uint32_t col=0; col < w; col++){
    *red   += pix_out[0][col] + 128.0;
    *green += pix_out[1][col] + 128.0;
    *blue  += pix_out[2][col] + 128.0;
  }
}

static
void compare_output(
  float r, float g, float b,
  statistics_t *stats)
{
  printf("mean [R] (hist, manual): (%.2f,%.2f)\n", stats->stats_red.mean, r);
  printf("mean [G] (hist, manual): (%.2f,%.2f)\n", stats->stats_green.mean, g);
  printf("mean [B] (hist, manual): (%.2f,%.2f)\n", stats->stats_blue.mean, b);
  TEST_ASSERT_UINT8_WITHIN (DELTA, r, stats->stats_red.mean);
  TEST_ASSERT_UINT8_WITHIN (DELTA, g, stats->stats_green.mean);
  TEST_ASSERT_UINT8_WITHIN (DELTA, b, stats->stats_blue.mean);
}

// Array initializers
static 
void filler_rand(int8_t pix_out[3][WIDTH])
{
  fill_array_rand_int8((int8_t *)pix_out, 3*WIDTH);
}
static
void filler_constant(int8_t pix_out[3][WIDTH])
{
  // Set each color a different value
  memset(pix_out[0], -50, WIDTH);
  memset(pix_out[1], 50,  WIDTH);
  memset(pix_out[2], 100, WIDTH);
}
static
void filler_edge(int8_t pix_out[3][WIDTH])
{
  // Set each color a different value
  memset(pix_out[0], -128, WIDTH);
  memset(pix_out[1],  127, WIDTH);
  memset(pix_out[2],    0, WIDTH);
}


// ---------------------- Tests ----------------------

statistics_t stats;
histograms_t histograms;

// create a generic function that takes a function pointer
void generic_test(void(*func)(int8_t [3][WIDTH]))
{
  stats_reset(&histograms, &stats);
  srand(time(NULL));
  float red = 0.0; float green = 0.0; float blue = 0.0;
    
  for (uint32_t row=0; row < HEIGHT; row++){

    int8_t pix_out[3][WIDTH];
    func(pix_out); // custom filler

    stats_compute_histograms(&histograms, WIDTH,pix_out); // histogram version
    manual_mean(WIDTH, pix_out, &red, &green, &blue); // manual version

    #if PRINT_HIST
      print_hist(&histograms.histogram_red);
    #endif
  }
  // Compute and compare
  red /= BUFFSIZE; green /= BUFFSIZE; blue /= BUFFSIZE;
  stats_compute_stats(&stats, &histograms, INV_BUFFSIZE);
  compare_output(red, green, blue, &stats);
}


TEST(stats_test, stats_test__basic){
  printf("Testing stats [random]\n");
  generic_test(filler_rand);
}
TEST(stats_test, stats_test__constants){
  printf("Testing stats [constant]\n");
  generic_test(filler_constant);
}
TEST(stats_test, stats_test__edge){
  printf("Testing stats [edge cases]\n");
  generic_test(filler_edge);
}
