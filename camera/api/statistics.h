
#pragma once 

#include <stdint.h>
#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdlib.h> // free, alloc

#include "xccompat.h"

#include "sensor.h"
#include "sensor_control.h"
#include "image_hfilter.h"

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

// Size of the channel element code space
#define CHANNEL_CARDINALITY           (1<<SENSOR_BIT_DEPTH)

// Number of histogram bins
#define HISTOGRAM_BIN_COUNT           ((CHANNEL_CARDINALITY) >> (APP_HISTOGRAM_QUANTIZATION_BITS))
#if (HISTOGRAM_BIN_COUNT != 64)
  #error HISTOGRAM_BIN_COUNT value not currently supported.
#endif

// Objects definitions
typedef struct {
  int8_t pixels[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_WIDTH_PIXELS];
} low_res_image_row_t;

typedef struct {
  int bins[HISTOGRAM_BIN_COUNT];
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

// Statistics compute funtions
void compute_skewness(channel_stats_t *stats);
void compute_simple_stats(channel_stats_t *stats);
void find_percentile(channel_stats_t *stats, const float fraction);


// Thread function
void statistics_thread(
    streaming_chanend_t c_img_in,
    CLIENT_INTERFACE(sensor_control_if, sc_if)
  );

#if defined(__XC__) || defined(__cplusplus)
}
#endif


