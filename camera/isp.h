#ifndef ISP_H
#define ISP_H

#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdint.h>

#include "statistics.h" // for dynamic AWB


// ----------------- ISP SETTINGS -------------------------------
#define MASK8           0xFF
#define MASK24          0x00FFFFFF
#define DEFAULT_ALFA    1.58682     // default red gain (not used for the moment)
#define DEFAULT_BETA    1.52535     // default blue gain (not used for the moment)
#define ENABLE_AE       1           // enable auto exposure 
#define AE_MARGIN       0.1         // default marging for the auto exposure error
#define STEP            16          // histogram step size

// ---------------------------------- AE/AGC ------------------------------
uint8_t csign(float x);
uint8_t isp_false_position_step(float exposure, float skewness);

// ---------------------------------- AWB ------------------------------
void isp_AWB_gray_world(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue);
void isp_AWB_percentile(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue);
void isp_AWB_static(const uint32_t buffsize, uint8_t *red, uint8_t *green, uint8_t *blue, float alfa, float beta, float delta);

// ---------------------------------- GAMMA ------------------------------
void isp_gamma_stride1(const uint32_t buffsize, uint8_t *img);
void isp_gamma_stride4(const uint32_t buffsize, uint8_t *img);

// -------------------------- ROTATE/RESIZE -------------------------------------
void isp_bilinear_resize(const uint16_t in_width, const uint16_t in_height, 
                        uint8_t *img, 
                        const uint16_t out_width, const uint16_t out_height, 
                        uint8_t *out_img);

void isp_rotate_image(const uint8_t* src, uint8_t* dest, int width, int height);

// -------------------------- COLOR CONVERSION -------------------------------------
// Macro arguments
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