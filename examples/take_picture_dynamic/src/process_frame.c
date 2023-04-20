
#include "process_frame.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define FINAL_IMAGE_FILENAME "img_raw.bin"

// Write image to disk. This is called by camera main () to do the work
void write_image(uint8_t *image)
{
  static FILE* img_file = NULL;
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");
  for(uint16_t k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
    for(uint16_t j = 0; j < MIPI_IMAGE_WIDTH_BYTES; j++){
      uint32_t pos = k * MIPI_LINE_WIDTH_BYTES + j;
      fwrite(&image[pos], sizeof(uint8_t), 1, img_file);
      }
  }
  fclose(img_file);
  printf("Outfile %s\n", FINAL_IMAGE_FILENAME);
  printf("image size (%dx%d)\n", MIPI_LINE_WIDTH_BYTES, MIPI_IMAGE_HEIGHT_PIXELS);
  free(image);
}


// This is called when want to memcpy from Xc to C
void c_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}

/*
void user_input(void){
  printf("Enter the character 'c' to capture the frame : ");
  int c = getchar();
  if(c != 99){ // == ascii "c"
    exit(0);
  }
}
*/