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
int8_t AWB_compute_filter_gain(int8_t coeff, float factor);


// ---------------------------------- GAMMA ------------------------------
extern const uint8_t gamma_1p8_s1[255];
void isp_gamma_stride1(const uint32_t buffsize, uint8_t *img);


// -------------------------- ROTATE/RESIZE -------------------------------------
void isp_bilinear_resize(
    const uint16_t in_width, 
    const uint16_t in_height, 
    uint8_t *img, 
    const uint16_t out_width, 
    const uint16_t out_height, 
    uint8_t *out_img);

void isp_rotate_image(const uint8_t *src, uint8_t *dest, int width, int height);

#endif // ISP_H
