
#include "process_frame.h"

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

const char* FINAL_IMAGE_FILENAME = "green.raw";
static FILE* img_file = NULL;

uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES] = {{0}};

// functions
void write_image()
{
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");

  for(int k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
    for(int j = 0; j < MIPI_IMAGE_WIDTH_PIXELS; j++){
      fwrite(&FINAL_IMAGE[k][j], sizeof(uint8_t), 1, img_file);
      }
  }
  fclose(img_file);
  printf("image written to %s\n", FINAL_IMAGE_FILENAME);
}

void not_silly_memcpy(
    void* dst,
    void* src,
    size_t size)
{
  memcpy(dst, src, size);
}