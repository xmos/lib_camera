#ifndef ISP_H
#define ISP_H

#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdint.h>

#include "statistics.h"

// black level is sensor dependant (used by horizontal filter)
#define BLACK_LEVEL 16

// ---------------------------------- AE/AGC ------------------------------
void AE_control_exposure(
    global_stats_t *global_stats,
    CLIENT_INTERFACE(sensor_control_if, sc_if));
void AE_print_skewness(global_stats_t *gstats);
float AE_compute_mean_skewness(global_stats_t *gstats);
uint8_t AE_is_adjusted(float sk);
uint8_t AE_compute_new_exposure(float exposure, float skewness);


// ---------------------------------- AWB ------------------------------
// Initial channel scales
#define AWB_gain_RED    1
#define AWB_gain_GREEN  1
#define AWB_gain_BLUE   1

/**
 * struct to hold the calculated parameters for the ISP
 */
typedef struct {
  float channel_gain[APP_IMAGE_CHANNEL_COUNT];
} isp_params_t;


extern isp_params_t isp_params;
void AWB_compute_gains(global_stats_t *gstats, isp_params_t *isp_params);
void AWB_print_gains(isp_params_t *isp_params);


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


/**
* Rotate the image by 90 degrees. This is useful for rotating images that are stored in a 3x3 array of uint8_t
* 
* @param filename - Name of the file to rotate
* @param image - Array of uint8_t that is to be
*/
void rotate_image_90(const char *filename, uint8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);

// -------------------------- COLOR CONVERSION -------------------------------------
// Macro arguments to get color components from packed result in the assembly program
#define GET_R(rgb) ((rgb >> 16) & 0xFF)
#define GET_G(rgb) ((rgb >> 8) & 0xFF)
#define GET_B(rgb) (rgb & 0xFF)

#define GET_Y(yuv) GET_R(yuv)
#define GET_U(yuv) GET_G(yuv)
#define GET_V(yuv) GET_B(yuv)

int yuv_to_rgb(
    int y, 
    int u, 
    int v);

int rgb_to_yuv(
    int r, 
    int g, 
    int b);

#endif // ISP_H
