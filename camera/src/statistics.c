// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>

#include "statistics.h"
#include "isp.h"
#include "print.h"


// Number of samples taken each row
#define HISTOGRAM_SAMPLE_PER_ROW       ((APP_IMAGE_WIDTH_PIXELS  + APP_HISTOGRAM_SAMPLE_STEP - 1)  / (APP_HISTOGRAM_SAMPLE_STEP))
// Number of samples taken in an image
#define HISTOGRAM_TOTAL_SAMPLES       (HISTOGRAM_SAMPLE_PER_ROW * APP_IMAGE_HEIGHT_PIXELS )
// This is the normalization factor
static const float histogram_norm_factor =  (1.0 / (float) HISTOGRAM_TOTAL_SAMPLES);

/**
* Update histogram based on pixel values. 
* 
* @param hist - * pointer to the histogram to update. Must be large enough to accommodate the number of pixels in the image.
* @param pix - array of pixel values to update the histogram with
*/
void stats_update_histogram(
    channel_histogram_t* hist,
    const int8_t pix[])
{
  for(int k = 0; k < APP_IMAGE_WIDTH_PIXELS; k += APP_HISTOGRAM_SAMPLE_STEP){
    int val = pix[k];
    val += 128; // convert from int8_t to uint8_t
    val >>= HIST_QUANT_BITS;
    hist->bins[val]++;
  }
}


/**
* Update histogram based on pixel values. 
* 
* @param hist - * pointer to the histogram to update. Must be large enough to accommodate the number of pixels in the image.
* @param pix - array of pixel values to update the histogram with
*/
void stats_update_histogram_new(
    channel_histogram_t* hist,
    const int8_t* pix,
    const size_t pix_size)
{
  for(uint32_t k = 0; k < pix_size; k ++){
    int8_t val = pix[k] + 128;
    val >>= HIST_QUANT_BITS; // we quantize the bins
    hist->bins[val]++;
  }
}

void stats_skewness(channel_stats_t *stats)
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
     0.740633,  0.821362,  0.907753,  1.0};

  float skew = 0.0;
  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    skew += zk_values[k] * stats->histogram.bins[k];
  }

  // Normnalization [3]
  stats -> skewness = skew * histogram_norm_factor;
}

void stats_simple(channel_stats_t *stats)
{
  // Calculate the histogram
  uint8_t temp_min = 0;
  uint8_t temp_max = 0;
  float temp_mean = 0;

  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    uint32_t bin_count = stats->histogram.bins[k];
    // mean
    temp_mean += bin_count * k;
    // max and min
    if (bin_count != 0){  // the last that is not zero
      temp_max = k;
      if (temp_min == 0){ // first time is zero, then min is set
        temp_min = k;
      }
    }
  }
  // max and min count
  stats->max_count = stats->histogram.bins[temp_max];
  stats->min_count = stats->histogram.bins[temp_min];

  // biased downwards due to truncation
  stats->max = (temp_max << HIST_QUANT_BITS);
  stats->min = (temp_min << HIST_QUANT_BITS);
  stats->mean = (temp_mean) *(1 << HIST_QUANT_BITS) * histogram_norm_factor;
}

void stats_print(channel_stats_t *stats, unsigned channel){
  const char* formattedString = "\nch:%d,Mi:%d,Ma:%d,Mean:%f,Sk:%f,pct:%d,mi_c:%lu,ma_c:%lu,pc:%lu\n";
  char output[255];  // Assuming a maximum length for the formatted string
  sprintf(output, formattedString,
      channel,
      stats->min,
      stats->max,
      stats->mean,
      stats->skewness,
      stats->percentile,
      stats->min_count,
      stats->max_count,
      stats->per_count);

  printstr(output);
}

void stats_percentile(channel_stats_t *stats, const float fraction)
{
  const unsigned threshold = fraction * HISTOGRAM_TOTAL_SAMPLES;
  // Could be optimized but fkeep it like this for timing reasons [2]
  uint8_t result;
  unsigned total = 0;

  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    unsigned new_total = total + stats->histogram.bins[k];
    if(total < threshold && new_total >= threshold)
      result = (k << HIST_QUANT_BITS);
    total = new_total;
  }
  stats -> percentile = (uint8_t) result;
}

void stats_percentile_volume(channel_stats_t *stats)
{
  uint32_t bin_count = 0;
  uint8_t percentile_point = stats -> percentile / 4;
  for(int k = percentile_point; k < HISTOGRAM_BIN_COUNT; k++){
    bin_count += stats->histogram.bins[k];
  }
  stats->per_count = bin_count;
}

// Notes
/*
[1]
The packet handler thread signals end-of-frame by sending a NULL
pointer to the statistics thread. Break out of inner loop on
end-of-frame.

[2]
This code can be written to exit early once the threshold is reached,
but that leads to a variable run time, depending upon the
characteristics of the image itself, which is highly undesirable. So,
I (astew) am writing it to have a run-time that doesn't depend on the
content of the image.

[3]
The skewness calculation assumes the histogram has been normalized into a
probability density-like distribution whose sum across all bins is 1.0.
That is just a matter of dividing each histogram bin by the total number of
sampled pixels (which is known a priori). Because every bin is adjusted by
the same factor, we can just wait to apply the adjustment until we get
here.  histogram_norm_factor is just the inverse of the total number of
sampled pixels.
*/
