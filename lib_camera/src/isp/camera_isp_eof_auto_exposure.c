// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <print.h>
#include <xcore/assert.h>

#include "camera_isp.h"

#define HIST_BIN_COUNT      (64)
#define HIST_QUANT_BITS     (2)

#define AE_MARGIN 0.1               // default marging for the auto exposure error
#define AE_INITIAL_EXPOSURE 35      // initial exposure value
#define AE_DONE 0

typedef enum {
    CHANNEL_RED = 0,
    CHANNEL_GREEN = 1,
    CHANNEL_BLUE = 2,
} channel_order_t;

typedef struct {
    uint32_t bins[HIST_BIN_COUNT];
} channel_histogram_t;

typedef struct {
    channel_histogram_t histogram_red;
    channel_histogram_t histogram_green;
    channel_histogram_t histogram_blue;
} histograms_t;

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

typedef struct {
    channel_stats_t stats_red;
    channel_stats_t stats_green;
    channel_stats_t stats_blue;
} statistics_t;


static
void stats_simple(
    channel_histogram_t* histogram,
    channel_stats_t* stats,
    const float inv_img_size)
{
    // Calculate the histogram
    uint8_t temp_min = 0;
    uint8_t temp_max = 0;
    float temp_mean = 0;

    for (int k = 0; k < HIST_BIN_COUNT; k++) {
        uint32_t bin_count = histogram->bins[k];
        // mean
        temp_mean += bin_count * k;
        // max and min
        if (bin_count != 0) {  // the last that is not zero
            temp_max = k;
            if (temp_min == 0) { // first time is zero, then min is set
                temp_min = k;
            }
        }
    }
    // max and min count
    stats->max_count = histogram->bins[temp_max];
    stats->min_count = histogram->bins[temp_min];

    // biased downwards due to truncation
    stats->max = (temp_max << HIST_QUANT_BITS);
    stats->min = (temp_min << HIST_QUANT_BITS);
    stats->mean = (temp_mean) * (1 << HIST_QUANT_BITS) * inv_img_size;
}

static
void stats_skewness(
    channel_histogram_t* histogram,
    channel_stats_t* stats,
    const float inv_img_size)
{
    const float zk_values[] = {
      -1.000000, -0.907753, -0.821362, -0.740633, -0.665375, -0.595396,
      -0.530504, -0.470508, -0.415214, -0.364431, -0.317968, -0.275632,
      -0.237231, -0.202574, -0.171468, -0.143721, -0.119142, -0.097538,
      -0.078717, -0.062488, -0.048659, -0.037037, -0.027431, -0.019648,
      -0.013497, -0.008786, -0.005323, -0.002915, -0.001372, -0.000005,
      -0.000108,    -4e-06,     4e-06,  0.000108,    0.0005,  0.001372,
       0.002915,  0.005323,  0.008786,  0.013497,  0.019648,  0.027431,
       0.037037,  0.048659,  0.062488,  0.078717,  0.097538,  0.119142,
       0.143721,  0.171468,  0.202574,  0.237231,  0.275632,  0.317968,
       0.364431,  0.415214,  0.470508,  0.530504,  0.595396,  0.665375,
       0.740633,  0.821362,  0.907753,  1.0 };

    float skew = 0.0;
    for (int k = 0; k < HIST_BIN_COUNT; k++) {
        skew += zk_values[k] * histogram->bins[k];
    }

    // Normnalization
    stats->skewness = skew * inv_img_size;
}

static inline
void stats_compute_stats(
    statistics_t* stats,
    histograms_t* histograms,
    const float inv_img_size)
{
    stats_simple(&histograms->histogram_red, &stats->stats_red, inv_img_size);
    stats_simple(&histograms->histogram_green, &stats->stats_green, inv_img_size);
    stats_simple(&histograms->histogram_blue, &stats->stats_blue, inv_img_size);

    stats_skewness(&histograms->histogram_red, &stats->stats_red, inv_img_size);
    stats_skewness(&histograms->histogram_green, &stats->stats_green, inv_img_size);
    stats_skewness(&histograms->histogram_blue, &stats->stats_blue, inv_img_size);
}

static inline
float stats_compute_mean_skewness(statistics_t* stats)
{
    float mean = \
        stats->stats_red.skewness + \
        stats->stats_green.skewness + \
        stats->stats_blue.skewness;
    mean /= 3.0;
    return mean;
}

static inline
int8_t csign(float x)
{
    return (x > 0) - (x < 0);
}

static
uint8_t AE_compute_new_exposure(float exposure, float skewness)
{
    static float a = 1;     // minimum value for exposure
    static float fa = -1;   // minimimum skewness
    static float b = 80;    // maximum value for exposure
    static float fb = 1;    // minimum skewness
    static int count = 0;
    float c = exposure;
    float fc = skewness;

    if (csign(fc) == csign(fa)) {
        a = c; fa = fc;
    }
    else {
        b = c; fb = fc;
    }
    c = b - fb * ((b - a) / (fb - fa));

    // each X samples, restart AE algorithm
    if (count < 5) {
        count = count + 1;
    }
    else {
        // restart auto exposure
        count = 0; a = 0; fa = -1; b = 80; fb = 1;
    }
    return c;
}

static inline
uint8_t AE_is_adjusted(float sk)
{
    return (sk < AE_MARGIN && sk > -AE_MARGIN) ? 1 : 0;
}

static
uint8_t AE_compute_exposure(
    statistics_t* global_stats)
{
    // Initial exposure
    static uint8_t new_exp = AE_INITIAL_EXPOSURE;
    static uint8_t skip_ae_control = 0; // if too dark for a ceertain frames, skip AE control

    // Compute skewness
    float sk = stats_compute_mean_skewness(global_stats);

    // Compute new exposure
    if (AE_is_adjusted(sk)) {
        return AE_DONE;
    }
    else {
        new_exp = AE_compute_new_exposure((float)new_exp, sk); // new_exp is in [1, 80]
        if (new_exp > 70) { // Skip AE control if too dark
            skip_ae_control++;
            if (skip_ae_control > 5) {
                skip_ae_control = 0;
                return AE_DONE;
            }
        }
    }
    return new_exp;
}


static
void stats_compute_hist_channel(
    channel_histogram_t* hist,
    image_cfg_t* image,
    channel_order_t channel)
{
    //TODO compute directly RGB histogram
    xassert(image->channels == 3 && "only RGB image supported");
    const uint32_t img_size = image->size;
    const int8_t* pix = (int8_t*)image->ptr;
    int16_t val = 0;
    for (uint32_t k = channel; k < img_size; k+=3) {
        val = pix[k];
        val += 128; // convert from int8_t to uint8_t
        val >>= HIST_QUANT_BITS;
        hist->bins[val]++;
    }
}

static inline
void stats_compute_histograms(
    histograms_t* histograms,
    image_cfg_t* image)
{
    stats_compute_hist_channel(&histograms->histogram_red, image, CHANNEL_RED);
    stats_compute_hist_channel(&histograms->histogram_green, image, CHANNEL_GREEN);
    stats_compute_hist_channel(&histograms->histogram_blue, image, CHANNEL_BLUE);
}


uint8_t camera_isp_auto_exposure(image_cfg_t* image)
{
    static histograms_t histograms;
    static statistics_t statistics;
    static uint8_t ae_value = 1;

    if (ae_value == AE_DONE) {
        return AE_DONE;
    }

    // compute histograms
    stats_compute_histograms(&histograms, image);

    // compute statistics
    const float inv_img_size = 1.0f / (image->width * image->height);
    stats_compute_stats(&statistics, &histograms, inv_img_size);

    // compute auto exposure
    ae_value = AE_compute_exposure(&statistics);

    // return to isp new value
    return ae_value;
}
