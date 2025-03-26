// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"

C_API_START

/**
 * @brief Dumps data into a file, can be lossy if done over xscope
 *
 * @param filename  Name of the file
 * @param data      Data to write
 * @param size      Size of the data
 */
void camera_io_write_file(char* filename, uint8_t* data, const size_t size);

/**
 * @brief Reads data into an array
 * 
 * @param filename Name of the file
 * @param data     Data to read to
 * @param size     Size of the file
 */
void camera_io_read_file(char * filename, uint8_t * data, const size_t size);

/**
 * @brief Writes binary image file
 *
 * @param filename  Name of the image
 * @param image     Pointer to the image data
 * @param height    Image height
 * @param width     Image width
 * @param channels  Number of channels
 * @note Image has to be in [height][width][channel] format
 */
void camera_io_write_image_file(char* filename, uint8_t* image, const size_t height, const size_t width, const size_t channels);

/**
 * @brief Writes bmp image file
 *
 * @param filename  Name of the image
 * @param image     Pointer to the image data
 * @param height    Image height
 * @param width     Image width
 * @param channels  Number of channels
 * @note Image has to be in [height][width][channel] format
 */
void camera_io_write_bmp_file(
    char* filename, 
    uint8_t* image, 
    const size_t height, 
    const size_t width, 
    const size_t channels);

/**
 * @brief Writes greyscale bmp image file
 * 
 * @param filename  Name of the image
 * @param image     Pointer to the image data
 * @param height    Image height
 * @param width     Image width
 * @note Image has to be in [height][width] format
 */
void write_bmp_greyscale(char * filename, uint8_t * image, const size_t height, const size_t width);

C_API_END
