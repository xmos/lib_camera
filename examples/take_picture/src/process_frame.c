
#include "process_frame.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define FINAL_IMAGE_FILENAME "out.raw"
uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES];


// Write image to disk. This is called by camera main () to do the work
void write_image()
{
  static FILE* img_file = NULL;
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");
  for(int k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
    for(int j = 0; j < MIPI_IMAGE_WIDTH_BYTES; j++){
      fwrite(&FINAL_IMAGE[k][j], sizeof(uint8_t), 1, img_file);
      }
  }
  fclose(img_file);
  printf("Outfile %s\n", FINAL_IMAGE_FILENAME);
  printf("image size (%dx%d)\n", MIPI_LINE_WIDTH_BYTES, MIPI_IMAGE_HEIGHT_PIXELS);
}


// This is called when want to memcpy from Xc to C
void not_silly_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}