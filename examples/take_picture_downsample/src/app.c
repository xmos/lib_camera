
#include "app.h"

#include <stdlib.h>
#include <stdint.h>
#include "io_utils.h"

void user_app(streaming_chanend_t c_cam_api){
  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];
  uint8_t temp_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];

  // set the input image to 0
  memset(image_buffer, -128, APP_IMAGE_CHANNEL_COUNT * APP_IMAGE_HEIGHT_PIXELS * APP_IMAGE_WIDTH_PIXELS);

  // Wait for the image to set exposure
  delay_milliseconds(5000);
  printf("Requesting image...\n");
  camera_capture_image(image_buffer, c_cam_api);
  printf("Image captured...\n");

  // Rotate 180 degrees
  // rotate_image(image_buffer);

  // convert to uint8 with right dimentions
  img_int8_to_uint8(image_buffer, temp_buffer);
  uint8_t * io_buff = (uint8_t *) &image_buffer[0][0][0]; // io_buff this will have [APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS][APP_IMAGE_CHANNEL_COUNT]
  swap_dimentions((uint8_t *) &temp_buffer[0][0][0], io_buff, APP_IMAGE_HEIGHT_PIXELS, APP_IMAGE_WIDTH_PIXELS, APP_IMAGE_CHANNEL_COUNT);

  // Write binary file and .bmp file
  write_image_file("capture.bin", io_buff, APP_IMAGE_HEIGHT_PIXELS, APP_IMAGE_WIDTH_PIXELS, APP_IMAGE_CHANNEL_COUNT);

  //save it to bmp
  writeBMP("capture.bmp", temp_buffer);

  // end here
  exit(0);
}
