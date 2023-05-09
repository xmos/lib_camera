#include <stdint.h>
#include <stdlib.h>

#ifndef STADISTICS_H
#define STADISTICS_H

typedef struct Stadistics{
    float histogram[64];
    float skewness;
    uint8_t mean;
    uint8_t max;
    uint8_t min;
} Stadistics;

Stadistics *Stadistics_alloc(void);
void Stadistics_free(Stadistics *self);

void Stadistics_compute_all(uint32_t buffsize, uint8_t *buffer, Stadistics *stadistics);
void Stadistics_compute_histogram(uint32_t buffsize, uint8_t *buffer, Stadistics *stadistics);
void Stadistics_compute_skewness(Stadistics *stadistics);
void Stadistics_compute_minmaxavg(Stadistics *stadistics);

// aux functions
static char get_rgb_color(uint32_t pos, uint16_t width);

#endif