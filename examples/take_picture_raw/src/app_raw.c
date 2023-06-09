// std
#include <stdio.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel_streaming.h>
// user
#include "mipi.h"
//#include "utils.h"
#include "user_api.h"
#include "app_raw.h"
#include "io_utils.h"

void user_app_raw(streaming_chanend_t c_cam_api){
    
    // set the input image to 0
    int8_t image_buffer[H_RAW * W_RAW];
    memset(image_buffer, -128, H_RAW * W_RAW);

    // wait for the camera to set I2C parameters
    delay_milliseconds(2500);

    // Request an image
    printf("Requesting image...\n");
    camera_capture_image_raw(image_buffer, c_cam_api);
    printf("Image captured...\n");
    
    // Save the image to a file
    write_image_file("capture.bin", (uint8_t * ) &image_buffer[0], MIPI_IMAGE_HEIGHT_PIXELS, MIPI_LINE_WIDTH_BYTES, 1);
    exit(0);
}
