
#include "process_frame.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include "image.h"


#define FINAL_IMAGE_FILENAME "img_raw.bin"
//uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS/2][MIPI_LINE_WIDTH_BYTES/2][3];
//uint8_t FINAL_IMAGE[(MIPI_IMAGE_HEIGHT_PIXELS/2)*(MIPI_LINE_WIDTH_BYTES/2)*3];
//uint8_t r_image[MIPI_IMAGE_HEIGHT_PIXELS/2*MIPI_LINE_WIDTH_BYTES/2];
//uint8_t g_image[MIPI_IMAGE_HEIGHT_PIXELS/2*MIPI_LINE_WIDTH_BYTES/2];
//uint8_t b_image[MIPI_IMAGE_HEIGHT_PIXELS/2*MIPI_LINE_WIDTH_BYTES/2];
uint8_t rgb_image[MIPI_IMAGE_HEIGHT_PIXELS/2*MIPI_LINE_WIDTH_BYTES/4][3];
// uint8_t img_rgb[CHEIGHT(FINAL_IMAGE)/2][CWIDTH(FINAL_IMAGE)/4][3];

// RAW to RGB pipeline
void raw_to_rgb(){
  // downsample image in raw
  // downsample_image[1][1] = 1;
  // img_rgb[1][1][1] = 1;
  // split the 3 color channels

  // Demosaic the image
}



// Write image to disk. This is called by camera main () to do the work
void write_image()
{
  static FILE* img_file = NULL;
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");
  for(int k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS; k++){
    for(int j = 0; j < MIPI_IMAGE_WIDTH_BYTES; j++){
      //fwrite(&FINAL_IMAGE[k][j], sizeof(uint8_t), 1, img_file);
      }
  }
  fclose(img_file);
  printf("Outfile %s\n", FINAL_IMAGE_FILENAME);
  // printf("image size (%dx%d)\n", MIPI_LINE_WIDTH_BYTES, MIPI_IMAGE_HEIGHT_PIXELS);
}

void write_image_rgb()
{
  static FILE* img_file = NULL;
  img_file = fopen(FINAL_IMAGE_FILENAME, "wb");
    for(int k = 0; k < MIPI_IMAGE_HEIGHT_PIXELS/2; k++){
      for(int j = 0; j < MIPI_IMAGE_WIDTH_BYTES/4; j++){
        int index = k * (MIPI_LINE_WIDTH_BYTES/4) + j;
        fwrite(&rgb_image[index][0], sizeof(uint8_t), 1, img_file);
        fwrite(&rgb_image[index][1], sizeof(uint8_t), 1, img_file);
        fwrite(&rgb_image[index][2], sizeof(uint8_t), 1, img_file);
    }
  }
  fclose(img_file);
  printf("Outfile %s\n", FINAL_IMAGE_FILENAME);
  // printf("image size (%dx%d)\n", MIPI_LINE_WIDTH_BYTES, MIPI_IMAGE_HEIGHT_PIXELS);
}


// This is called when want to memcpy from Xc to C
void not_silly_memcpy(
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