// std
#include <stdio.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel_streaming.h>
// user
#include "mipi.h"
#include "user_api.h"
#include "app_raw.h"
#include "io_utils.h"

void user_app_raw(){
    
    // set the input image to 0
    int8_t image_buffer[H_RAW][W_RAW];
    memset(image_buffer, -128, H_RAW * W_RAW);

    // wait for the camera to set I2C parameters
    delay_milliseconds(5000);

    // Request an image
    printf("Requesting image...\n");
    if(camera_capture_image_raw(image_buffer)){
        printf("Error capturing image\n");
        exit(1);
    }
    printf("Image captured...\n");

    // Convert image from int8 to uint8 in-place
    vect_int8_to_uint8((uint8_t*) image_buffer,
                      (int8_t*) image_buffer, 
                      sizeof(image_buffer));

    // Save the image to a file
    write_image_file("capture.bin", (uint8_t * ) &image_buffer[0][0],   
            MIPI_IMAGE_HEIGHT_PIXELS, MIPI_LINE_WIDTH_BYTES, 1);

    printf("Image saved. Exiting.\n");
    xscope_close_all_files();
    exit(0);
}
