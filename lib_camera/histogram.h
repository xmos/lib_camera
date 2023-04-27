#include <stdio.h>
#include <stdint.h>


// histograms
void compute_hist_32(uint32_t buffsize, uint8_t *buffer, float histogram[32]);
void compute_hist_64_norm(uint32_t buffsize, uint8_t *buffer, float histogram[64]);
void compute_hist_64_doub(uint32_t buffsize, uint8_t *buffer, float histogram[64]);
void compute_hist_64_norm_pointer(uint32_t buffsize, uint8_t *buffer, float histogram[64]);
void compute_hist_64_norm_pointer2(uint32_t buffsize, uint8_t *buffer, float histogram[64]);
void compute_hist_64_norm_pointer3(uint32_t buffsize, uint8_t *buffer, float histogram[64]);

// skewness of the histogram
float skew_fast(float* hist);
