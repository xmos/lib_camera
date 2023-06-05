#ifndef ISP_H
#define ISP_H

#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdint.h>

#include "statistics.h"

#define DEFAULT_ALFA 1.58682
#define DEFAULT_BETA 1.52535


// ---------------------------------- AE/AGC ------------------------------
float AE_compute_mean_skewness(global_stats_t *gstats);
uint8_t AE_is_adjusted(float sk);
uint8_t AE_compute_new_exposure(float exposure, float skewness);

// ---------------------------------- AWB ------------------------------
typedef struct {
    float alfa;
    float beta;
    float gamma;
} AWB_gains_t;

void AWB_compute_gains(global_stats_t *gstats, AWB_gains_t *gains);
void AWB_print_gains(AWB_gains_t *gains);

#endif