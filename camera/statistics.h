#ifndef STAtISTICS_H
#define STAtISTICS_H

#include <stdint.h>
#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdlib.h> // free, alloc

typedef struct Statistics{
    float histogram[64];
    float skewness;
    uint8_t mean;
    uint8_t max;
    uint8_t min;
    uint8_t percentile;
} Statistics;

Statistics *Statistics_alloc(void);
Statistics Statistics_initialize(void);
void Statistics_free(Statistics *self);

void Statistics_compute_all(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Statistics *statistics);
void Statistics_compute_histogram(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Statistics *statistics);
void Statistics_compute_skewness(Statistics *statistics);
void Statistics_compute_minmaxavg(Statistics *statistics);
void Statistics_compute_percentile(Statistics *statistics);
uint16_t Statistics_compute_variance(Statistics *statistics);

// aux functions
void Statistics_print_info(Statistics *statistics);

#endif