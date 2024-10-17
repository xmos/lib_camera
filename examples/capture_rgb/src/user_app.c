// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xcore/channel.h>
#include <xcore/select.h>

#include "camera.h"
#include "camera_utils.h"
#include "camera_isp.h"
#include "camera_conv.h"
#include "camera_io.h"

#define H   200
#define W   200
#define CH    3 // RGB
#define DELAY_MILISECONDS 100

#define OUTPUT_INT8 1

static
void sim_model_invoke() {
    printf("Simulating model\n");
    delay_milliseconds_cpp(DELAY_MILISECONDS);
}

static
void save_image(image_cfg_t* image, char* filename) {
    uint8_t * img_ptr = (uint8_t*)image->ptr;
    #if (OUTPUT_INT8 == 0)
        camera_int8_to_uint8(img_ptr, image->ptr, image->size);
    #endif
    camera_io_write_image_file(filename, img_ptr, H, W, CH); // this will close the file as well
}


void user_app(chanend_t c_cam) {

    // Image and configuration
    int8_t image_buffer[H][W][CH] = {{{0}}};
    int8_t* image_ptr = &image_buffer[0][0][0];

    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_RGB1,
    };
    image_cfg_t image = {
        .height = H,
        .width = W,
        .channels = CH,
        .size = H * W * CH,
        .ptr = image_ptr,
        .config = &config
    };

    // wait a few seconds and ask somthing
    delay_seconds_cpp(3);
    
    // From here, it could be a while loop
    
    // set coords and send to ISP
    camera_isp_coordinates_compute(&image);
    camera_isp_start_capture(c_cam, &image);
    sim_model_invoke(); // this is just some big delay to show that it is non-blocking
    camera_isp_get_capture(c_cam);
    save_image(&image, "capture1.rgb");

    // change coordinates
    /*
    config.offset_x = 0.5;
    config.offset_y = 0.1;
    camera_isp_coordinates_compute(&image);
    camera_isp_start_capture(c_cam, &image);
    sim_model_invoke(); // this is just some big delay to show that it is non-blocking
    camera_isp_get_capture(c_cam);
    save_image(&image, "capture2.raw");
    */

    // (Optional) try somthing makes no sense
    /*
    config.offset_x = 0.5; // 640*0.5 + 400 (width) = 720 > 640 !!
    config.offset_y = 0.5; // 480*0.5 + 300 (height) = 540 > 480 !!
    camera_isp_coordinates_compute(&image);
    camera_isp_coordinates_print(&image);
    */
    camera_io_exit();
    exit(0);
}
