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

// -------------------------- COLOR CONVERSION -------------------------------------
// Macro arguments
#define MASK8           0xFF
#define MASK24          0x00FFFFFF
#define GET_R(x) ((int8_t)((x & MASK24)) & MASK8)
#define GET_G(x) ((int8_t)((x & MASK24) >> 8) & MASK8)
#define GET_B(x) ((int8_t)((x & MASK24) >> 16) & MASK8)
#define GET_Y(x) GET_R(x)
#define GET_U(x) GET_G(x)
#define GET_V(x) GET_B(x)

// Converts signed yuv or rgb (-127..127, -127..127, -127..127) into signed rgb / yuv. 
extern int yuv_to_rgb(int y, int u, int v);
extern int rgb_to_yuv(int r, int g, int b);

#endif // ISP_H
