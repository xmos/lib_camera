// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

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

    // Create an Image
    int8_t image_buffer[H][W][CH];
    int8_t* image_ptr = &image_buffer[0][0][0];
    Image_t image = {
        .height = H,
        .width = W,
        .channels = CH,
        .ptr = image_ptr
    }; //TODO this will include camera config as well

    // wait a few seconds and ask somthing
    delay_seconds_cpp(10);

    while (1) {
        //unsigned rx = chan_in_word(c_user); // wait for the image
        //printf("Received image %d\n", rx);
        camera_isp_capture_in_ms(c_user, 160, &image);
        sim_model_invoke();
    }


}
