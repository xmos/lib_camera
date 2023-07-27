#pragma once

#include <string.h> // memset
#include <stdio.h>  // null 
#include <stdint.h>

#include "statistics.h" // needed for global_stats_t

// ISP settings
#define AE_MARGIN 0.1          // default marging for the auto exposure error
#define AE_INITIAL_EXPOSURE 35 // initial exposure value
#define AWB_gain_RED    1.3
#define AWB_gain_GREEN  1
#define AWB_gain_BLUE   1.3
#define AWB_MAX         1.7
#define AWB_MIN         0.8
#define APPLY_GAMMA     1
#define ENABLE_PRINT_STATS 0

// ---------------------------------- ISP PIPELINE ----------------------------------
void isp_pipeline(
    streaming_chanend_t c_img_in,
    chanend c_control);
// ---------------------------------- AE/AGC ------------------------------

/**
 * @brief auto exposure control funciton
 * 
 * @param global_stats structure containing the global statistics
 */
uint8_t AE_control_exposure(
    global_stats_t *global_stats,
    chanend c_exp);

/**
 * @brief aux function to print the skewness
 *
 * @param gstats structure containing the global statistics
 */
void AE_print_skewness(global_stats_t *gstats);

/**
 * @brief function to compute the mean skewness
 *
 * @param gstats structure containing the global statistics
 * @return float skewness normalized from (-1,1)
 */
float AE_compute_mean_skewness(global_stats_t *gstats);

/**
 * @brief checks if the skewness is inside the desired interval
 *
 * @param sk skewness
 * @return uint8_t 1 if the skewness is inside the interval, 0 otherwise
 */
uint8_t AE_is_adjusted(float sk);

/**
 * @brief computes the new exposure value
 * it uses false position step for computing the exposure
 * https://research.ijcaonline.org/volume83/number14/pxc3892895.pdf
 * @param exposure
 * @param skewness
 * @return uint8_t
 */
uint8_t AE_compute_new_exposure(float exposure, float skewness);

// ---------------------------------- AWB ------------------------------

/**
 * struct to hold the calculated parameters for the ISP
 */
typedef struct {
  float channel_gain[APP_IMAGE_CHANNEL_COUNT];
} isp_params_t;

// current isp parameters for white balancing
extern isp_params_t isp_params;

/**
 * @brief auto white balance control function
 * 
 * @param isp_params structure containing the current isp parameters
 */
void AWB_compute_gains_static(isp_params_t *isp_params);

/**
 * @brief auto white balance control function based on white patch algorithm
 * 
 * @param gstats structure containing the global statistics
 * @param isp_params structure containing the current isp parameters
 */
void AWB_compute_gains_white_patch(global_stats_t *gstats, isp_params_t *isp_params);

/**
 * @brief auto white balance control function based on gray world algorithm
 * 
 * @param gstats structure containing the global statistics
 * @param isp_params structure containing the current isp parameters
 */
void AWB_compute_gains_gray_world(global_stats_t *gstats, isp_params_t *isp_params);

/**
 * @brief auto white balance control function based on white max algorithm
 * 
 * @param gstats   structure containing the global statistics
 * @param isp_params structure containing the current isp parameters
 */
void AWB_compute_gains_white_max(global_stats_t *gstats, isp_params_t *isp_params);

/**
 * @brief auto white balance control function based on percentile
 * 
 * @param gstats structure containing the global statistics
 * @param percentile percentile to compute
 */
void AWB_compute_gains_percentile(global_stats_t *gstats, isp_params_t *isp_params);

/**
 * @brief aux function to print the auto white balancing gains
 * 
 * @param isp_params structure containing the current isp parameters
 */
void AWB_print_gains(isp_params_t *isp_params);

// ---------------------------------- GAMMA ------------------------------
// Gamma correction table
extern const uint8_t gamma_uint8[256];
extern const int8_t  gamma_int8[256];

/**
 * @brief compute gamma curve given an image and the gamma adjustement
 * 
 * @param img_in pointer to the image
 * @param gamma_curve pointer to the gamma curve
 * @param height image height
 * @param width image width
 * @param channels channels in the image
 */
void isp_gamma(uint8_t *img_in, const uint8_t *gamma_curve, const size_t height, const size_t width, const size_t channels);

// -------------------------- ROTATE/RESIZE -------------------------------------

/**
 * @brief bilinear interpolation function
 * 
 * @param in_width   width of the input image
 * @param in_height  height of the input image
 * @param img        pointer to the input image
 * @param out_width  output width
 * @param out_height output height
 * @param out_img    pointer to the output image
 */
void isp_bilinear_resize(
    const uint16_t in_width, 
    const uint16_t in_height, 
    uint8_t *img, 
    const uint16_t out_width, 
    const uint16_t out_height, 
    uint8_t *out_img);

/**
* Rotate the image by 90 degrees. This is useful for rotating images that are stored in a 3x3 array of uint8_t
* 
* @param image - Array of uint8_t that is to be
*/
void rotate_image_90(uint8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);

// -------------------------- COLOR CONVERSION -------------------------------------
// Macro arguments to get color components from packed result in the assembly program
#define GET_R(rgb) (rgb & 0xFF)
#define GET_G(rgb) ((rgb >> 8) & 0xFF)
#define GET_B(rgb) ((rgb >> 16)& 0xFF)

#define GET_Y(yuv) GET_R(yuv)
#define GET_U(yuv) GET_G(yuv)
#define GET_V(yuv) GET_B(yuv)

/**
 * @brief converts a YUV pixel to RGB
 * 
 * @param y Y component
 * @param u U component
 * @param v V component
 * @return int result of rgb conversion (need macros to decode output)
 */
int yuv_to_rgb(
    int y, 
    int u, 
    int v);

/**
 * @brief converts a RGB pixel to YUV
 * 
 * @param r red component
 * @param g green component
 * @param b blue component
 * @return int result of yuv conversion (need macros to decode output)
 */
int rgb_to_yuv(
    int r, 
    int g, 
    int b);
