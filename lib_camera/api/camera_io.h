// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include "api.h"
#include "xscope_io_device.h"

C_API_START

/**
 * @brief Opens a file
 *
 * @param filename Name of the file
 * @note The application has to end with xscope_close_all_files()
 */
void camera_io_fopen(const char* filename);

/**
 * @brief Fills an array with data from a file opened with camera_io_fopen()
 *
 * @param data Pointer to the data to be filled
 * @param size Size of the data
 */
void camera_io_fill_array_from_file(uint8_t* data, const size_t size);

/**
 * @brief Rewinds a file opened with camera_io_fopen() (seek 0)
 *
 */
void camera_io_rewind_file();

/**
 * @brief Dumps data into a file
 *
 * @param filename  Name of the file
 * @param data      Data to write
 * @param size      Size of the data
 * @note The application has to end with xscope_close_all_files()
 */
void camera_io_write_file(char* filename, uint8_t* data, const size_t size);

/**
 * @brief Writes binary image file
 *
 * @param filename  Name of the image
 * @param image     Pointer to the image data
 * @param height    Image height
 * @param width     Image width
 * @param channels  Number of channels
 * @note Image has to be in [height][width][channel] format
 * @note The application has to end with xscope_close_all_files()
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
 * @note The application has to end with xscope_close_all_files()
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
 * @note The application has to end with xscope_close_all_files()
 */
void write_bmp_gs(char * filename, uint8_t * image, const size_t height, const size_t width);

C_API_END
