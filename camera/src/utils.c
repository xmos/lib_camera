
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


/**
* Write image to a binary file containing RGB data
* 
* @param filename -Name of the image
* @param image - Image corresponding to a 3D array of uint8_t 
*/
void write_image(
    const char* filename,
    uint8_t image[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS])
{
  printf("Writing image...\n");
  static FILE* img_file = NULL;
  img_file = fopen(filename, "wb");

  
    for(uint16_t k = 0; k < APP_IMAGE_HEIGHT_PIXELS; k++){
      for(uint16_t j = 0; j < APP_IMAGE_WIDTH_PIXELS; j++){
        for(uint8_t c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++){
          fwrite(&image[c][k][j], sizeof(uint8_t), 1, img_file);
      }
    }
  }
  fclose(img_file);
  printf("Outfile %s\n", filename);
  printf("image size (%dx%d)\n", APP_IMAGE_WIDTH_PIXELS, APP_IMAGE_HEIGHT_PIXELS);
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


/**
* Writes BMP image to file. This function is used to write a bmp image to a file.
* 
* @param filename - Name of file to write to. The file must end with. bmp
* @param img - Array of uint8_t that contains the image
*/
void writeBMP(const char* filename, uint8_t img[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS]) {
    int width = APP_IMAGE_WIDTH_PIXELS;
    int height = APP_IMAGE_HEIGHT_PIXELS;
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

    // Open the file for writing
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error opening file for writing: %s\n", filename);
        return;
    }

    // Write the BMP file header and info header
    fwrite(bmpFileHeader, sizeof(unsigned char), 14, file);
    fwrite(bmpInfoHeader, sizeof(unsigned char), 40, file);

    // Write the image data row by row (with padding)
    int i, j;
    for (i = height - 1; i >= 0; i--) {
        for (j = 0; j < width; j++) {
            // Write the pixel data (assuming RGB order)
            fwrite(&img[2][i][j], sizeof(unsigned char), 1, file); // Blue
            fwrite(&img[1][i][j], sizeof(unsigned char), 1, file); // Green
            fwrite(&img[0][i][j], sizeof(unsigned char), 1, file); // Red
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
    printf("image size (%dx%d)\n", APP_IMAGE_WIDTH_PIXELS, APP_IMAGE_HEIGHT_PIXELS);
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
