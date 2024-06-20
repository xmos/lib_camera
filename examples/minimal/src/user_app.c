// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xcore/channel.h>
#include <xcore/select.h>

#include "lib_camera.h"
#include "camera_utils.h"
#include "camera_isp.h"
#include "camera_io_utils.h"

#define H   480
#define W   640
#define CH    1 // RAW

static
void sim_model_invoke() {
    printf("Simulating model\n");
    delay_milliseconds_cpp(1000);
}

static
void save_image(Image_cfg_t* image) {
    uint8_t * img_ptr = (uint8_t*)image->ptr;
    size_t size = image->height * image->width * image->channels;

    vect_int8_to_uint8(img_ptr, (int8_t*)image->ptr, size);
    write_image_file("capture.raw", img_ptr, H, W, CH);
}


void user_app(chanend_t c_cam[N_CH_USER_ISP]) {
    // channel unpack
    chanend_t c_user_isp = c_cam[CH_USER_ISP];
    chanend_t c_isp_user = c_cam[CH_ISP_USER];

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
    };

    // Create an Image
    int8_t __attribute__((aligned(8))) image_buffer[H][W][CH];
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

    static unsigned just_once = 1;

    // User app loop
    SELECT_RES(
        CASE_THEN(c_isp_user, on_c_isp_user),
        DEFAULT_THEN(on_user_app))
    {
        on_c_isp_user: {
            uint8_t data = chan_in_byte(c_isp_user);
            printf("Image recieved\n");
            printf("Data: %d\n", data);
            save_image(&image);
            xscope_close_all_files();
            break;
        }
        on_user_app: {
            if (just_once) {
                printf("Sending configuration\n");
                camera_isp_send_cfg(c_user_isp, &image);
                just_once = 0;
            }
            sim_model_invoke(); // this is just some delay to show is non-blocking
            continue;
        }
    }
    exit(0);
}
