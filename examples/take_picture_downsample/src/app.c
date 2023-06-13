
#include "app.h"

#include <stdlib.h>
#include <stdint.h>

void user_app()
{
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];

  // set the input image to 0
  memset(image_buffer, -128, sizeof(image_buffer));

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
  vect_int8_to_uint8((uint8_t*) image_buffer,
                     (int8_t*) image_buffer, 
                     sizeof(image_buffer));

  // Write binary file and .bmp file
  write_image("capture.bin", 
              (uint8_t*) image_buffer,
              APP_IMAGE_CHANNEL_COUNT, 
              APP_IMAGE_HEIGHT_PIXELS, 
              APP_IMAGE_WIDTH_PIXELS);

  //save it to bmp
  writeBMP("capture.bmp", 
           (uint8_t*) image_buffer,
           APP_IMAGE_HEIGHT_PIXELS, 
           APP_IMAGE_WIDTH_PIXELS);

  // end here
  exit(0);
}
