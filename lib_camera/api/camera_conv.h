// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stddef.h>


// -------------------- Color -----------------------





// -------------------- Dtype -----------------------

/**
 * Convert an array of int8 to an array of uint8.
 * Data can be updated in-place.
 *
 * @param output - Array of uint8_t that will contain the output
 * @param input - Array of int8_t that contains the input
 * @param length - Length of the input and output arrays
 */
    void camera_conv_int8_to_uint8(
        uint8_t output[],
        int8_t input[],
        const unsigned length);

// -------------------- Spatial -----------------------

/**
 * @brief Swaps image dimensions from [channel][height][width]
 * to [height][width][channel]
 *
 * @param image_in  Input image
 * @param image_out Output image
 * @param height    Image height
 * @param width     Image width
 * @param channels  Number of channels
 */
void camera_conv_swap_dims(
    uint8_t* image_in, 
    uint8_t* image_out, 
    const size_t height, 
    const size_t width, 
    const size_t channels);

/**
 * @brief Rotates an RGB image 90 degrees clockwise 
 * assumes both src and dst images are previously allocated
 * Input image dimensions are [height][width][channel]
 * and output image dimensions are [width][height][channel]
 * 
 * @param dst_img pointer to the destination image
 * @param src_img pointer to the source image
 * @param h height of the input image
 * @param w width of the input image
 * @param ch channels of the input image
 */
void camera_conv_rotate90_uint8(
    uint8_t* dst_img, 
    uint8_t* src_img, 
    const int16_t h, 
    const int16_t w, 
    const int16_t ch);

/**
 * @brief Rotates an RGB image 90 degrees clockwise 
 * assumes both src and dst images are previously allocated
 * Input image dimensions are [height][width][channel]
 * and output image dimensions are [width][height][channel]
 * 
 * @param dst_img pointer to the destination image
 * @param src_img pointer to the source image
 * @param h height of the input image
 * @param w width of the input image
 * @param ch channels of the input image
 */
void camera_conv_rotate90_int8(
    int8_t* dst_img, 
    int8_t* src_img, 
    const int16_t h, 
    const int16_t w, 
    const int16_t ch);
