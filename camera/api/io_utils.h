// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __XC__
extern "C" {
#endif

#include "xscope_io_device.h"


/**
 * @brief Opens a file
 * 
 * @param filename Name of the file
 * @note The application has to end with xscope_close_all_files()
 */
void io_open_file(const char* filename);

/**
 * @brief Fills an array with data from a file opened with io_open_file()
 * 
 * @param data Pointer to the data to be filled
 * @param size Size of the data
 */
void io_fill_array_from_file(uint8_t* data, const size_t size);

/**
 * @brief Rewinds a file opened with io_open_file() (seek 0)
 * 
 */
void io_rewind_file();

/**
 * @brief Dumps data into a file
 * 
 * @param filename  Name of the file
 * @param data      Data to write
 * @param size      Size of the data
 * @note The application has to end with xscope_close_all_files()
 */
void write_file(char * filename, uint8_t * data, const size_t size);

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
void swap_dimensions(uint8_t * image_in, uint8_t * image_out, const size_t height, const size_t width, const size_t channels);

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
void write_image_file(char * filename, uint8_t * image, const size_t height, const size_t width, const size_t channels);

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
void write_bmp_file(char * filename, uint8_t * image, const size_t height, const size_t width, const size_t channels);

#ifdef __XC__
}
#endif
