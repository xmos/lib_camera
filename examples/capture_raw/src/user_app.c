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

#define H   300
#define W   400
#define CH    1 // RAW
#define DELAY_MILISECONDS 100

static
void sim_model_invoke() {
    printf("Simulating model\n");
    delay_milliseconds_cpp(DELAY_MILISECONDS);
}

static
void save_image(image_cfg_t* image, char* filename) {
    uint8_t * img_ptr = (uint8_t*)image->ptr;
    size_t size = image->height * image->width * image->channels;

    vect_int8_to_uint8(img_ptr, (int8_t*)image->ptr, size);
    io_write_image_file(filename, img_ptr, H, W, CH); // this will close the file as well
}


void user_app(chanend_t c_cam[N_CH_USER_ISP]) {
    // channel unpack
    chanend_t c_user_to_isp = c_cam[CH_USER_ISP];
    chanend_t c_isp_to_user = c_cam[CH_ISP_USER];

    // Create a Configuration
    camera_configure_t config = {
        .offset_x = 0.2,
        .offset_y = 0.1,
        .sx = 1, // Note: in raw sx sy has to be 1
        .sy = 1,
        .shx = 0.0,
        .shy = 0.0,
        .angle = 0.0,
        .T = NULL,
    };

    // Create an Image Structure
    int8_t __attribute__((aligned(8))) image_buffer[H][W][CH] = {{{0}}};
    int8_t* image_ptr = &image_buffer[0][0][0];
    image_cfg_t image = {
        .height = H,
        .width = W,
        .channels = CH,
        .ptr = image_ptr,
        .config = &config
    };

    // wait a few seconds and ask somthing
    delay_seconds_cpp(3);
    
    // From here, it could be a while loop
    
    // set coords and send to ISP
    camera_isp_coordinates_compute(&image);
    camera_isp_coordinates_print(&image);
    chan_out_buf_byte(c_user_to_isp, (uint8_t*)&image, sizeof(image_cfg_t));
    sim_model_invoke(); // this is just some big delay to show that it is non-blocking
    chan_in_byte(c_isp_to_user); // wait for the image
    save_image(&image, "capture1.raw");

    // change coordinates
    config.offset_x = 0.1;
    config.offset_y = 0.3;
    camera_isp_coordinates_compute(&image);
    camera_isp_coordinates_print(&image);
    chan_out_buf_byte(c_user_to_isp, (uint8_t*)&image, sizeof(image_cfg_t));
    sim_model_invoke(); // this is just some big delay to show that it is non-blocking
    chan_in_byte(c_isp_to_user); // wait for the image
    save_image(&image, "capture2.raw");

    // (Optional) try somthing makes no sense
    /*
    config.offset_x = 0.5; // 640*0.5 + 400 (width) = 720 > 640 !!
    config.offset_y = 0.5; // 480*0.5 + 300 (height) = 540 > 480 !!
    camera_isp_coordinates_compute(&image);
    camera_isp_coordinates_print(&image);
    */

    xscope_close_all_files();
    exit(0);
}
