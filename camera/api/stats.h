#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcore/assert.h>
#include <xcore/channel.h> // includes streaming channel and channend
#include <xccompat.h>

#include "sensor.h"

#define HISTOGRAM_BIN_COUNT 64


typedef struct {
  uint32_t bins[HISTOGRAM_BIN_COUNT];
} channel_histogram_t;

typedef struct {
  uint8_t min;
  uint8_t max;
  uint8_t percentile;
  float skewness;
  float mean;
  uint32_t max_count;
  uint32_t min_count;
  uint32_t per_count;
} channel_stats_t;

// Group structs
typedef struct{
  channel_histogram_t histogram_red;
  channel_histogram_t histogram_green;
  channel_histogram_t histogram_blue;
} histograms_t;

typedef struct{
  channel_stats_t stats_red;
  channel_stats_t stats_green;
  channel_stats_t stats_blue;
} statistics_t;

void stats_compute_histograms(
    histograms_t* histograms, 
    const int8_t pix_out[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS], 
    const uint32_t pixel_size);

void stats_reset(histograms_t* histograms, statistics_t* stats);
void stats_compute_stats(statistics_t* stats, histograms_t* histograms, const float inv_img_size);
float stats_compute_mean_skewness(statistics_t* stats);
