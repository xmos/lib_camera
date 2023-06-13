
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


// This is called when want to memcpy from Xc to C
void c_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}


/**
* Rotate the image by 90 degrees. This is useful for rotating images that are stored in a 3x3 array of uint8_t
* 
* @param filename - Name of the file to rotate
* @param image - Array of uint8_t that is to be
*/
void rotate_image(
  const char* filename,
  uint8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS])
{
  for(int c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++) {
    for(int k = 0; k < APP_IMAGE_HEIGHT_PIXELS/2; k++) {
      for(int j = 0; j < APP_IMAGE_WIDTH_PIXELS; j++) {
        uint8_t a = image_buffer[c][k][j];
        uint8_t b = image_buffer[c][APP_IMAGE_HEIGHT_PIXELS-k-1][APP_IMAGE_WIDTH_PIXELS-j-1];
        image_buffer[c][k][j] = b;
        image_buffer[c][APP_IMAGE_HEIGHT_PIXELS-k-1][APP_IMAGE_WIDTH_PIXELS-j-1] = a;
      }
    } 
  }
}

/**
 * Convert an array of int8 to an array of uint8.
 * 
 * Data can be updated in-place.
 */
void vect_int8_to_uint8(
    uint8_t output[],
    int8_t input[], 
    const unsigned length)
{
  for(int k = 0; k < length; k++)
    output[k] = input[k] + 128;
}