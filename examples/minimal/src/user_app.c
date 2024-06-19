// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "lib_camera.h"
#include "camera_utils.h"
#include "camera_isp.h"

#include <xcore/channel.h>

#define H 120
#define W 160
#define CH 3

static
void sim_model_invoke() {
    printf("Simulating model\n");
    delay_milliseconds_cpp(1000);
}

void user_app(chanend_t c_user) {
    // Create configuration
    camera_configure_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .sx = 1.0,
        .sy = 1.0,
        .shx = 0.0,
        .shy = 0.0,
        .angle = 0.0,
        .T = NULL,
        .delay = 200, // 200ms
        .cmd = SENSOR_STREAM_START
    };

    // Create an Image
    int8_t image_buffer[H][W][CH];
    int8_t* image_ptr = &image_buffer[0][0][0];
    Image_cfg_t image = {
        .height = H,
        .width = W,
        .channels = CH,
        .ptr = image_ptr,
        .config = &config
    };

    // wait a few seconds and ask somthing
    delay_seconds_cpp(3);

    // in a loop send cfg and simulate model
    // change cfg at some point
    unsigned counter = 0;
    while (1) {
        
        camera_isp_send_cfg(c_user, &image);
        sim_model_invoke(); // this is just some delay to show is non-blocking

        if (counter++ >= 10){
            printf("Changing delay\n");
            image.config->delay = 20; // we reduce the delay
        }

        // if counter is 20 we stop
        if (counter >= 20) {
            image.config->cmd = SENSOR_STREAM_STOP;
            camera_isp_send_cfg(c_user, &image);
        }
    }


}
