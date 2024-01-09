// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xcore/assert.h>

#include "sensor.h"

#define HISTOGRAM_BIN_COUNT (64)
#define HIST_QUANT_BITS     (2)

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

/**
 * @brief Compute the histograms for each channel
 * 
 * @param histograms histogram struct pointer
 * @param width      width of the image
 * @param pix_out    pointer to the image row (3 channels)
 */
void stats_compute_histograms(
  histograms_t* histograms, 
  const uint32_t width, 
  const int8_t pix_out[3][width]);

/**
 * @brief Reset to zero the histograms and stats
 */
void stats_reset(histograms_t* histograms, statistics_t* stats);

/**
 * @brief               Compute the stats for each channel
 * @param stats         stats struct pointer
 * @param histograms    histogram struct pointer
 * @param inv_img_size  nomalization factor 1.0 / (width * height)
 */
void stats_compute_stats(statistics_t* stats, histograms_t* histograms, const float inv_img_size);

/**
 * @brief Print the stats
 */
void stats_print(statistics_t* stats);

/**
 * @brief         Compute the mean skewness
 * @param stats   stats struct pointer
 * @return float  mean skewness
 */
float stats_compute_mean_skewness(statistics_t* stats);
