
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void write_image(
    const char* filename,
    uint8_t image[],
    const unsigned channels,
    const unsigned height,
    const unsigned width)
{
  printf("Writing image...\n");

  int8_t (*img3d)[height][width] = (int8_t (*)[height][width]) image;

  static FILE* img_file = NULL;
  img_file = fopen(filename, "wb");
  
  for(uint16_t k = 0; k < height; k++){
    for(uint16_t j = 0; j < width; j++){
      for(uint8_t c = 0; c < channels; c++){
        fwrite(&img3d[c][k][j], sizeof(uint8_t), 1, img_file);
      }
    }
  }
  fclose(img_file);
  printf("Outfile %s\n", filename);
  printf("image size (%dx%d)\n", width, height);
}



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

static
int writeBMPHeader(
    FILE* file,
    const unsigned height,
    const unsigned width)
{
  int channels = APP_IMAGE_CHANNEL_COUNT;
  
  // Define BMP file header and info header
  unsigned char bmpFileHeader[14] = {
      'B', 'M', // Signature
      0, 0, 0, 0, // File size (to be filled later)
      0, 0, // Reserved
      0, 0, // Reserved
      54, 0, 0, 0 // Offset to image data
  };

  unsigned char bmpInfoHeader[40] = {
      40, 0, 0, 0, // Info header size
      0, 0, 0, 0, // Image width (to be filled later)
      0, 0, 0, 0, // Image height (to be filled later)
      1, 0, // Number of color planes
      8 * channels, 0, // Bits per pixel
      0, 0, 0, 0, // Compression method
      0, 0, 0, 0, // Image size (to be filled later)
      0, 0, 0, 0, // Horizontal resolution (pixel per meter)
      0, 0, 0, 0, // Vertical resolution (pixel per meter)
      0, 0, 0, 0, // Number of colors in the palette
      0, 0, 0, 0, // Number of important colors
  };
  
  // Calculate the row size (including padding)
  int rowSize = width * channels;
  int paddingSize = (4 - (rowSize % 4)) % 4;
  int rowSizeWithPadding = rowSize + paddingSize;

  // Calculate the file size
  int fileSize = 54 + (rowSizeWithPadding * height);

  // Update the file size in the BMP file header
  bmpFileHeader[2] = (unsigned char)(fileSize);
  bmpFileHeader[3] = (unsigned char)(fileSize >> 8);
  bmpFileHeader[4] = (unsigned char)(fileSize >> 16);
  bmpFileHeader[5] = (unsigned char)(fileSize >> 24);

  // Update the image width in the BMP info header
  bmpInfoHeader[4] = (unsigned char)(width);
  bmpInfoHeader[5] = (unsigned char)(width >> 8);
  bmpInfoHeader[6] = (unsigned char)(width >> 16);
  bmpInfoHeader[7] = (unsigned char)(width >> 24);

  // Update the image height in the BMP info header
  bmpInfoHeader[8] = (unsigned char)(height);
  bmpInfoHeader[9] = (unsigned char)(height >> 8);
  bmpInfoHeader[10] = (unsigned char)(height >> 16);
  bmpInfoHeader[11] = (unsigned char)(height >> 24);

  fwrite(bmpFileHeader, sizeof(unsigned char), 14, file);
  fwrite(bmpInfoHeader, sizeof(unsigned char), 40, file);

  return paddingSize;
}



void writeBMP(
    const char* filename, 
    uint8_t img[],
    const unsigned height,
    const unsigned width)
{
  int8_t (*img3d)[height][width] = (int8_t (*)[height][width]) img;

  // Open the file for writing
  FILE* file = fopen(filename, "wb");
  if (!file) {
    printf("Error opening file for writing: %s\n", filename);
    return;
  }

  // Write the BMP file header and info header
  int paddingSize = writeBMPHeader(file, height, width);

  // Write the image data row by row (with padding)
  int i, j;
  for (i = height - 1; i >= 0; i--) {
    for (j = 0; j < width; j++) {
      // Write the pixel data (assuming RGB order)
      fwrite(&img3d[2][i][j], sizeof(unsigned char), 1, file); // Blue
      fwrite(&img3d[1][i][j], sizeof(unsigned char), 1, file); // Green
      fwrite(&img3d[0][i][j], sizeof(unsigned char), 1, file); // Red
      // For 4-channel images, you can write the alpha channel here
      // fwrite(&img[index + 3], sizeof(unsigned char), 1, file); // Alpha
    }

    // Write the padding bytes
    for (j = 0; j < paddingSize; j++) {
      fwrite("\0", sizeof(unsigned char), 1, file);
    }
  }

  // Close the file
  fclose(file);
  printf("Outfile %s\n", filename);
  printf("image size (%dx%d)\n", width, height);
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