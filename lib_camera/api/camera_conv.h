// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "api.h"

C_API_START

// -------------------- Color -----------------------

/**
 * @brief Convert an RGB image to the greyscale one.
 * The operation can be performed safely in-place on the same pointer.
 * `n_pix` must be a multiple of 4.
 *
 * @param gs_img    Greyscale image
 * @param img       RGB image
 * @param n_pix     Number of RGB pixels
 */
void camera_rgb_to_greyscale4(
	int8_t * gs_img,
	int8_t * img,
	unsigned n_pix);

/**
 * @brief Convert an RGB image to the greyscale one.
 * The operation can be performed safely in-place on the same pointer.
 * `n_pix` must be a multiple of 16.
 *
 * @param gs_img    Greyscale image
 * @param img       RGB image
 * @param n_pix     Number of RGB pixels
 */
void camera_rgb_to_greyscale16(
	int8_t * gs_img,
	int8_t * img,
	unsigned n_pix);

/**
 * @brief Convert an RGB image to the greyscale one.
 * The operation can be performed safely in-place on the same pointer.
 * `n_pix` must be a multiple of 4.
 *
 * @param gs_img    Greyscale image
 * @param img       RGB image
 * @param n_pix     Number of RGB pixels
 */
inline void camera_rgb_to_greyscale(
	int8_t * gs_img,
	int8_t * img,
	unsigned n_pix) {
	camera_rgb_to_greyscale4(gs_img, img, n_pix);
}

// Macro arguments to get color components from packed result
#define GET_R(rgb) (rgb & 0xFF)
#define GET_G(rgb) ((rgb >> 8) & 0xFF)
#define GET_B(rgb) ((rgb >> 16)& 0xFF)

#define GET_Y(yuv) GET_R(yuv)
#define GET_U(yuv) GET_G(yuv)
#define GET_V(yuv) GET_B(yuv)

/**
 * @brief Converts a YUV pixel to RGB.
 *
 * @param y         Y component
 * @param u         U component
 * @param v         V component
 * @return int result of rgb conversion (need macros to decode output)
 */
int camera_yuv_to_rgb(
    int y,
    int u,
    int v);

/**
 * @brief Converts a RGB pixel to YUV.
 *
 * @param r         red component
 * @param g         green component
 * @param b         blue component
 * @return int result of yuv conversion (need macros to decode output)
 */
int camera_rgb_to_yuv(
    int r,
    int g,
    int b);

// -------------------- Dtype -----------------------

/**
 * @brief Convert an array of int8 to an array of uint8.
 * Data can be updated in-place.
 *
 * @param output    Array of uint8_t that will contain the output
 * @param input     Array of int8_t that contains the input
 * @param length    Length of the input and output arrays
 */
void camera_int8_to_uint8(
    uint8_t * output,
    int8_t * input,
    const unsigned length);

// -------------------- Spatial -----------------------

/**
 * @brief Swaps image dimensions from [x][y][z]
 * to [y][z][x]
 *
 * @param image_in  Input image
 * @param image_out Output image
 * @param height    Image height
 * @param width     Image width
 * @param channels  Number of channels
 */
void camera_swap_dims(
    uint8_t* image_in,
    uint8_t* image_out,
    const unsigned height,
    const unsigned width,
    const unsigned channels);

/**
 * @brief Rotates an RGB image 90 degrees clockwise,
 * assumes both src and dst images are previously allocated,
 * and input image pointer can't be reused for the output image.
 * Input image dimensions are [height][width][channel],
 * and only int8 or uint8 types are supported.
 * Output image dimensions are [width][height][channel],
 * and they are of the same type as the input image.
 *
 * @param dst_img   Destination image
 * @param src_img   Source image (only uint8 or int8 types are supported)
 * @param h         Image height
 * @param w         Image width
 * @param ch        Number of channels
 */
void camera_rotate90(
    void* dst_img,
    void* src_img,
    const int16_t h,
    const int16_t w,
    const int16_t ch);

C_API_END
