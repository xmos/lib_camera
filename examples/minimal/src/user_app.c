// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include "delay.h"

#include <xcore/channel.h>

static void get_image_in_ms(chanend_t c_user, unsigned ms){
    chan_out_word(c_user, ms); // to isp_packet_handler
}

void sim_model_invoke(){
    printf("Simulating model\n");
    delay_milliseconds_cpp(1000);
}

void user_app(chanend_t c_user){

    // wait a few seconds and ask somthing
    delay_seconds_cpp(10);

    while (1)
    {
        //unsigned rx = chan_in_word(c_user); // wait for the image
        //printf("Received image %d\n", rx);
        get_image_in_ms(c_user, 160); // ask for an image (1s delay
        sim_model_invoke();
    }
    
    
}
