// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

// std
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <xcore/assert.h>

#include "camera_io_utils.h"
#include "app.h"

void user_app()
{
  // Initialize camera api
  camera_init();

  int8_t image_buffer[H][W][CH];
  uint8_t * image_ptr = (uint8_t *) &image_buffer[0][0][0];

  // set the input image to 0
  memset(image_buffer, -128, sizeof(image_buffer));

  // Wait for the image to set exposure
  delay_milliseconds(4000);

  // grab a frame
  printf("Requesting image...\n");
  xassert((camera_capture_image(image_buffer) == 0) && "Could not capture an image");
  printf("Image captured...\n");

  // stop the threads and camera stream
  camera_stop();
  delay_milliseconds(100);

  
  // convert to uint8
  vect_int8_to_uint8(image_ptr,
                     &image_buffer[0][0][0],
                     sizeof(image_buffer));
  
  // Write binary file
  write_image_file("capture.bin",
                    image_ptr,
                    APP_IMAGE_HEIGHT_PIXELS,
                    APP_IMAGE_WIDTH_PIXELS,
                    APP_IMAGE_CHANNEL_COUNT);

  // Write bmp file
  write_bmp_file("capture.bmp",
                  image_ptr,
                  APP_IMAGE_HEIGHT_PIXELS,
                  APP_IMAGE_WIDTH_PIXELS,
                  APP_IMAGE_CHANNEL_COUNT);

  printf("Images saved. Exiting.\n");
  xscope_close_all_files();
  exit(0);
}
