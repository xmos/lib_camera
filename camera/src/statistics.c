

#include <xcore/channel_streaming.h>
#include "xccompat.h"

#include "statistics.h"
#include "isp.h"

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
void update_histogram(
    channel_histogram_t* hist,
    const int8_t pix[])
{
  for(int k = 0; k < APP_IMAGE_WIDTH_PIXELS; k += APP_HISTOGRAM_SAMPLE_STEP){
    int val = pix[k];
    val += 128; // convert from int8_t to uint8_t
    val >>= APP_HISTOGRAM_QUANTIZATION_BITS;
    hist->bins[val]++;
  }
}



/**
* Compute skewness of channel. 
* This is used by auto exposure
* @param stats - * Pointer to channel statistics to update.
*/
void compute_skewness(channel_stats_t *stats)
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




/**
* Compute simple statistics for a set of data. 
* @param stats - * Pointer to the channel statistics to be computed
*/
void compute_simple_stats(channel_stats_t *stats)
{
  // Calculate the histogram
  uint8_t temp_min = 0;
  uint8_t temp_max = 0;

  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    unsigned bin = stats->histogram.bins[k];
    // mean
    stats->mean += bin * k;
    // max and min
    if (bin != 0){ 
      temp_max = k;
      if (temp_min == 0){
        temp_min = k;
      }
    }
    //stats->max = (stats->max >= bin)? stats->max : bin;
    //stats->min = (stats->min <= bin)? stats->min : bin;
  }
  stats->max = temp_max;
  stats->min = temp_min;
  // biased downwards due to truncation
  stats->max <<= APP_HISTOGRAM_QUANTIZATION_BITS;
  stats->min <<= APP_HISTOGRAM_QUANTIZATION_BITS;
  stats->mean *= (1<<APP_HISTOGRAM_QUANTIZATION_BITS) * histogram_norm_factor;
}


/**
 * Find the value for which (fraction) portion of pixels fall below that value. 
 */
void find_percentile(channel_stats_t *stats, const float fraction)
{
  const unsigned threshold = fraction * HISTOGRAM_TOTAL_SAMPLES;
  // Could be optimized but fkeep it like this for timing reasons [2]
  uint8_t result;
  unsigned total = 0;

  for(int k = 0; k < HISTOGRAM_BIN_COUNT; k++){
    unsigned new_total = total + stats->histogram.bins[k];
    if(total < threshold && new_total >= threshold)
      result = (k << APP_HISTOGRAM_QUANTIZATION_BITS);
    total = new_total;
  }
  stats -> percentile = (uint8_t) result;
}


/**
* Thread that computes statistics for each pixel in the image. 
* The statistics are stored in a struct which is used to perform
* isp corrections
* @param c_img_in - Channel end of the
*/
void statistics_thread(
    streaming_chanend_t c_img_in,
    CLIENT_INTERFACE(sensor_control_if, sc_if))
{
  // Outer loop iterates over frames
  while(1){
    global_stats_t global_stats = {{0}};
    // Inner loop iterates over rows within a frame
    while(1){

      low_res_image_row_t* row = (low_res_image_row_t*) s_chan_in_word(c_img_in);

      if(row == NULL) // Signal end of frame [1]
        break;

      // Update histogram
      for(uint8_t channel = 0; channel < APP_IMAGE_CHANNEL_COUNT; channel++){
        update_histogram(&global_stats[channel].histogram, &row->pixels[channel][0]);
      }
    }
    
    // End of frame, compute statistics
    for(uint8_t channel = 0; channel < APP_IMAGE_CHANNEL_COUNT; channel++){
      compute_skewness(&global_stats[channel]);
      compute_simple_stats(&global_stats[channel]);
      find_percentile(&global_stats[channel], APP_WB_PERCENTILE);
    }

    // Adjust AE
    AE_control_exposure(&global_stats, sc_if);

    // Adjust AWB 
    AWB_compute_gains(&global_stats, &isp_params);
    
    // Print ISP info
    AWB_print_gains(&isp_params);
    AE_print_skewness(&global_stats);
  }
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
