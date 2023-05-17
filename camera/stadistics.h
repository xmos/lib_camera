#ifndef STADISTICS_H
#define STADISTICS_H

#include <stdint.h>
#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdlib.h> // free, alloc

typedef struct Stadistics{
    float histogram[64];
    float skewness;
    uint8_t mean;
    uint8_t max;
    uint8_t min;
    uint8_t percentile;
} Stadistics;

Stadistics *Stadistics_alloc(void);
Stadistics Stadistics_initialize(void);
void Stadistics_free(Stadistics *self);

void Stadistics_compute_all(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Stadistics *stadistics);
void Stadistics_compute_histogram(const uint32_t buffsize, const uint8_t step, uint8_t *buffer, Stadistics *stadistics);
void Stadistics_compute_skewness(Stadistics *stadistics);
void Stadistics_compute_minmaxavg(Stadistics *stadistics);
void Stadistics_compute_percentile(Stadistics *stadistics);
uint16_t Stadistics_compute_variance(Stadistics *stadistics);

// aux functions
static char get_rgb_color(uint32_t pos, uint16_t width);

#endif