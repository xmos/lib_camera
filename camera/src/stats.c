#include "stats.h"


static
void compute_hist_channel(
    channel_histogram_t* hist,
    const int8_t* pix, 
    const uint32_t pixel_size)
{
  int16_t val = 0;
  for(uint32_t k = 0; k < pixel_size; k ++){
    val = pix[k];
    val += 128; // convert from int8_t to uint8_t
    val >>= HIST_QUANT_BITS;
    hist->bins[val]++;
  }
}

static
void stats_simple(channel_histogram_t* histogram, channel_stats_t* stats, const float inv_img_size)
{
  // Calculate the histogram
  uint8_t temp_min = 0;
  uint8_t temp_max = 0;
  float temp_mean  = 0;

  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    uint32_t bin_count = histogram->bins[k];
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
  stats->max_count = histogram->bins[temp_max];
  stats->min_count = histogram->bins[temp_min];

  // biased downwards due to truncation
  stats->max = (temp_max << HIST_QUANT_BITS);
  stats->min = (temp_min << HIST_QUANT_BITS);
  stats->mean = (temp_mean) * inv_img_size;
}

static
void stats_skewness(channel_histogram_t* histogram, channel_stats_t* stats, const float inv_img_size)
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
    skew += zk_values[k] * histogram->bins[k];
  }

  // Normnalization
  stats -> skewness = skew * inv_img_size;
}


// ---------------------- Statistics ----------------------
void stats_compute_histograms(
    histograms_t *histograms,
    const int8_t pix_out[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS], 
    const uint32_t pixel_size)
{
    compute_hist_channel(&histograms->histogram_red,   pix_out[0], pixel_size);
    compute_hist_channel(&histograms->histogram_green, pix_out[1], pixel_size);
    compute_hist_channel(&histograms->histogram_blue,  pix_out[2], pixel_size);
}

void stats_reset(
    histograms_t* histograms,
    statistics_t* stats)
{
    memset(histograms, 0, sizeof(histograms_t));
    memset(stats, 0, sizeof(statistics_t));
}


void stats_compute_stats(
    statistics_t* stats,
    histograms_t* histograms,
    const float inv_img_size)
{
    stats_simple(&histograms->histogram_red,   &stats->stats_red,   inv_img_size);
    stats_simple(&histograms->histogram_green, &stats->stats_green, inv_img_size);
    stats_simple(&histograms->histogram_blue,  &stats->stats_green, inv_img_size);

    stats_skewness(&histograms->histogram_red,   &stats->stats_red,   inv_img_size);
    stats_skewness(&histograms->histogram_green, &stats->stats_green, inv_img_size);
    stats_skewness(&histograms->histogram_blue,  &stats->stats_green, inv_img_size);
}   

float stats_compute_mean_skewness(statistics_t *stats)
{
  float mean = \
    stats->stats_red.skewness + \
    stats->stats_green.skewness + \
    stats->stats_blue.skewness;
  mean /= 3.0;
  return mean;
}
