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
#include "utils_new.h"

void user_app_raw(streaming_chanend_t c_cam_api){
    
    // set the input image to 0
    int8_t image_buffer[H_RAW*W_RAW];
    memset(image_buffer, -128, H_RAW*W_RAW);

    // wait for set the camera
    delay_milliseconds(500);

    // Request an image
    printf("Requesting image...\n");
    camera_capture_image_raw(image_buffer, c_cam_api);
    printf("Image captured...\n");
    
    // Save the image to a file
    printf("before writing mage\n");
    //write_image_raw("capture.bin", image_buffer);
    write_image_new("capture.bin", image_buffer, MIPI_IMAGE_HEIGHT_PIXELS, MIPI_LINE_WIDTH_BYTES);
    exit(0);
}
