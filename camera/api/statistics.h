
#pragma once 

#include <stdint.h>
#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdlib.h> // free, alloc
#include <stdint.h>

#include "xccompat.h"

#include "sensor.h"
#include "sensor_control.h"
#include "image_hfilter.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif


#define CH_CARD               (1<<SENSOR_BIT_DEPTH)     // Size of the channel element code space
#define HISTOGRAM_BIN_COUNT   ((CH_CARD) >> (HIST_QUANT_BITS))  // Number of histogram bins
#define APP_WB_PERCENTILE     (0.94)

#if (HISTOGRAM_BIN_COUNT != 64)
  #error HISTOGRAM_BIN_COUNT value not currently supported.
#endif

// ---------- Objects definitions ----------
typedef struct {
  int8_t pixels[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
} low_res_image_row_t;

typedef struct {
  uint32_t bins[HISTOGRAM_BIN_COUNT];
} channel_histogram_t;

typedef struct {
  uint8_t min;
  uint8_t max;
  uint8_t percentile;
  float skewness;
  float mean;
  channel_histogram_t histogram;
} channel_stats_t;

typedef channel_stats_t global_stats_t[APP_IMAGE_CHANNEL_COUNT];


// ---------- Api functions ----------

/**
 * @brief Update the histogram given a row of pixels
 * 
 * @param hist current histogram
 * @param pix row of pixels
 */
void stats_update_histogram(channel_histogram_t *hist, const int8_t pix[]);

/**
* Compute simple statistics for a set of data. 
* @param stats - * Pointer to the channel statistics to be computed
*/
void stats_simple(channel_stats_t *stats);

/**
 * @brief Compute the skewness of a channel
 * 
 * @param stats 
 */
void stats_skewness(channel_stats_t *stats);

/**
 * Find the value for which (fraction) portion of pixels fall below that value. 
 */
void stats_percentile(channel_stats_t *stats, const float fraction);

/**
 * @brief print the statistics of a channel
 * 
 * @param stats 
 * @param channel 
 */
void stats_print(channel_stats_t *stats, unsigned channel);




#if defined(__XC__) || defined(__cplusplus)
}
#endif
