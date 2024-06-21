// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include "api.h"
#include "xscope_io_device.h"

C_API_START

// -------------------- Conversions -----------------------

/**
 * Convert an array of int8 to an array of uint8.
 *
 * Data can be updated in-place.
 *
 * @param output - Array of uint8_t that will contain the output
 * @param input - Array of int8_t that contains the input
 * @param length - Length of the input and output arrays
 */
    void vect_int8_to_uint8(
        uint8_t output[],
        int8_t input[],
        const unsigned length);

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
void swap_dimensions(uint8_t* image_in, uint8_t* image_out, const size_t height, const size_t width, const size_t channels);

// -------------------- Delays -----------------------

void delay_ticks_cpp(unsigned ticks);
void delay_milliseconds_cpp(unsigned delay);
void delay_seconds_cpp(unsigned int delay);

// -------------------- IO -----------------------

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
void io_write_file(char* filename, uint8_t* data, const size_t size);

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
void io_write_image_file(char* filename, uint8_t* image, const size_t height, const size_t width, const size_t channels);

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
void io_write_bmp_file(
    char* filename, 
    uint8_t* image, 
    const size_t height, 
    const size_t width, 
    const size_t channels);


C_API_END
