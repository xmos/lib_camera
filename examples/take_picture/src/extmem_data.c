
#include "extmem_data.h"

#include <string.h>
#include <stdio.h>


// EXTMEM_DATA
// __attribute__((aligned(4)))
// uint8_t image_capture[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_IMAGE_WIDTH_PIXELS][2] = {{{0}}};
// uint8_t* image_capture = 0x10000000;


// Simply bypasses the (unnecessary) protections on calling memcpy() from XC.
void not_silly_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}

static
FILE* img_file = NULL;

void write_image()
{
  // img_file = fopen("captured_image.bin", "wb");

  // for(int k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
  //   for(int j = 0; j < MIPI_IMAGE_WIDTH_PIXELS; j++){
  //     for(int m = 0; m < 2; m++){
  //       fwrite(&image_capture[k][j][m], 1, 1, img_file);
  //     }
  //   }
  // }
  
  // fclose(img_file);
}