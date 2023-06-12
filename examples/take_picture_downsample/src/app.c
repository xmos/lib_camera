
#include "app.h"

#include <stdlib.h>
#include <stdint.h>

void user_app()
{
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];
  uint8_t out_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];

  // set the input image to 0
  memset(image_buffer, -128, APP_IMAGE_CHANNEL_COUNT * APP_IMAGE_HEIGHT_PIXELS * APP_IMAGE_WIDTH_PIXELS);

  // Wait for the image to set exposure
  delay_milliseconds(5000);
  printf("Requesting image...\n");
  if(camera_capture_image(image_buffer)){
    printf("Error capturing image\n");
    exit(1);
  }
  printf("Image captured...\n");

  // Rotate 180 degrees
  // rotate_image(image_buffer);

  // convert to uint8
  img_int8_to_uint8(image_buffer, out_buffer);

  // Write binary file and .bmp file
  write_image("capture.bin", out_buffer);

  //save it to bmp
  writeBMP("capture.bmp", out_buffer);

  // end here
  exit(0);
}
