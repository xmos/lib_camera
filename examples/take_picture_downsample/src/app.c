
#include "app.h"

#include <stdlib.h>
#include <stdint.h>

void user_app(streaming_chanend_t c_cam_api){
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];
  uint8_t out_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];

  for(int8_t k = 0; k < APP_IMAGE_CHANNEL_COUNT; k++){
    memset(&image_buffer[k][0][0], -128, APP_IMAGE_HEIGHT_PIXELS * APP_IMAGE_WIDTH_PIXELS); // important to set to 0 in int8 (-128)
  }
  
  delay_milliseconds(5000);
  printf("Requesting image...\n");
  camera_capture_image(image_buffer, c_cam_api);
  printf("Image captured...\n");

  // Add 128 to all elements
  for(uint16_t c = 0; c < APP_IMAGE_CHANNEL_COUNT; c++){
    for(uint16_t k = 0; k < APP_IMAGE_HEIGHT_PIXELS; k++){
      for(uint16_t j = 0; j < APP_IMAGE_WIDTH_PIXELS; j++){
        out_buffer[c][k][j] =  (uint8_t)(image_buffer[c][k][j] + 128);
      }
    }
  }
        
  // Rotate 180 degrees
  #if ROTATE_IMAGE
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
  #endif

  // Write binary file and .bmp file
  write_image("capture.bin", out_buffer);
  writeBMP("capture.bmp", out_buffer);

  // end here
  exit(0);
}
