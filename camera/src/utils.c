
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

// Convert int8_t to uint8_t image
void img_int8_to_uint8(
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS],
  uint8_t out_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]
)
{
  // Add 128 to all elements
  for(uint16_t c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++){
    for(uint16_t k = 0; k < APP_IMAGE_HEIGHT_PIXELS; k++){
      for(uint16_t j = 0; j < APP_IMAGE_WIDTH_PIXELS; j++){
        int8_t val = image_buffer[c][k][j];
        out_buffer[c][k][j] = val + 128;
      }
    }
  }
}


// Convert int8_t to uint8_t image
void img_int8_to_uint8_inplace(int8_t image_buffer[H_RAW][W_RAW])
{
  // Add 128 to all elements
  for (int i = 0; i < H_RAW; i++) {
      for (int j = 0; j < W_RAW; j++) {
          image_buffer[i][j] += 128;
      }
  }
}
