#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "sensor.h"

#define PRINT_TIME(a,b) printf("%d\n", b - a);

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

static inline
unsigned measure_time(){
  unsigned y = 0;
  asm volatile("gettime %0": "=r"(y));
  return y;
}

/**
* Writes BMP image to file. This function is used to write a bmp image to a
* file.
* 
* @param filename - Name of file to write to. The file must end with. bmp
* @param img - Array of uint8_t that contains the image
* @param height - Height of the image
* @param width - Width of the image
*/
void writeBMP(
    const char* filename, 
    uint8_t img[],
    const unsigned height,
    const unsigned width);


/**
* Write image to a binary file containing RGB data
* 
* @param filename -Name of the image
* @param image - Image corresponding to a 3D array of uint8_t 
* @param channels - Number of channels in the image
* @param height - Height of the image
* @param width - Width of the image
*/
void write_image(
    const char* filename,
    uint8_t image[],
    const unsigned channels,
    const unsigned height,
    const unsigned width);


void c_memcpy(void *dst, void *src, size_t size);

void rotate_image(
    const char *filename, 
    uint8_t image[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]);



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

#if defined(__XC__) || defined(__cplusplus)
}
#endif
