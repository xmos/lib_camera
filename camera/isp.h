#ifndef ISP_H
#define ISP_H

#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdint.h>

#include "stadistics.h" // for dynamic AWB
//#include "utils.h"

#define DEFAULT_ALFA 1.58682
#define DEFAULT_BETA 1.52535


// ---------------------------------- AE/AGC ------------------------------
uint8_t csign(float x);
uint8_t isp_false_position_step(float exposure, float skewness);

// ---------------------------------- AWB ------------------------------
void isp_AWB_gray_world(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue);
void isp_AWB_percentile(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue);
void isp_AWB_static(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue, float alfa, float beta, float delta);

// ---------------------------------- GAMMA ------------------------------
void isp_gamma_4(const uint32_t buffsize, uint8_t *img);
void isp_gamma_1(const uint32_t buffsize, uint8_t *img);

#endif