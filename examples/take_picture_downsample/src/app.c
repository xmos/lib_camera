#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "io_utils.h"
#include "app.h"
#include "isp.h"  // needed for gamma

void user_app()
{
  // Initialize camera api
  camera_init();

  int8_t image_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];
  uint8_t temp_buffer[APP_IMAGE_CHANNEL_COUNT][APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS];

  // set the input image to 0
  memset(image_buffer, -128, sizeof(image_buffer));

  // Wait for the image to set exposure
  delay_milliseconds(4000);
  printf("Requesting image...\n");
  // grab a frame
  if(camera_capture_image(image_buffer)){
    printf("Error capturing image\n");
    exit(1);
  }
  printf("Image captured...\n");

  // stop the threads and camera stream
  camera_stop();
  delay_milliseconds(100);

  // convert to uint8 with right dimentions
  // convert to uint8
  vect_int8_to_uint8((uint8_t*) image_buffer,
                     (int8_t*) image_buffer, 
                     sizeof(image_buffer));

  memcpy(temp_buffer, image_buffer, APP_IMAGE_CHANNEL_COUNT * APP_IMAGE_HEIGHT_PIXELS * APP_IMAGE_WIDTH_PIXELS * sizeof(uint8_t));
  uint8_t * io_buff = (uint8_t *) &image_buffer[0][0][0];
  // io_buff this will have [APP_IMAGE_HEIGHT_PIXELS][APP_IMAGE_WIDTH_PIXELS][APP_IMAGE_CHANNEL_COUNT] dimentions
  
  // apply gamma correction
  #if APPLY_GAMMA
    isp_gamma((uint8_t *) &temp_buffer[0][0][0],
                          &gamma_new[0], 
                          APP_IMAGE_HEIGHT_PIXELS,
                          APP_IMAGE_WIDTH_PIXELS,
                          APP_IMAGE_CHANNEL_COUNT);
  #endif
  
  swap_dimensions((uint8_t *) &temp_buffer[0][0][0], io_buff,
                    APP_IMAGE_HEIGHT_PIXELS,
                    APP_IMAGE_WIDTH_PIXELS,
                    APP_IMAGE_CHANNEL_COUNT);

  // Write binary file
  write_image_file("capture.bin", io_buff,
                    APP_IMAGE_HEIGHT_PIXELS,
                    APP_IMAGE_WIDTH_PIXELS,
                    APP_IMAGE_CHANNEL_COUNT);

  // Write bmp file
  write_bmp_file("capture.bmp", io_buff,
                  APP_IMAGE_HEIGHT_PIXELS,
                  APP_IMAGE_WIDTH_PIXELS,
                  APP_IMAGE_CHANNEL_COUNT);

  printf("Images saved. Exiting.\n");
  xscope_close_all_files();
  // end here
  exit(0);
}
