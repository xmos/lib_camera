// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <xscope.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <xcore/parallel.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_io.h"
#include "camera_conv.h"

#define TO_MS(ticks) ((float)(ticks) / XS1_TIMER_KHZ)

#define H   200
#define W   200
#define CH  3

#ifndef FILE_IN_NAME
#define FILE_IN_NAME "src/imgs/capture0_int8.raw"
#endif

#ifndef FILE_OUT_NAME
#define FILE_OUT_NAME "src/imgs/capture0_int8_out.rgb"
#endif

#ifndef USE_OPTIMISED
#define USE_OPTIMISED 0
#endif

#if USE_OPTIMISED == 0
#define CAMERA_ISP_RAW8_RGB1 camera_isp_raw8_to_rgb1_baseline
#elif USE_OPTIMISED == 1
#define CAMERA_ISP_RAW8_RGB1 camera_isp_raw8_to_rgb1
#endif

void test_isp() {    
    printf("[test_isp]\n");

    // Create a Configuration
    int8_t image_buffer[H][W][CH] ALIGNED_8 = { {{0}} };
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
    camera_isp_coordinates_compute(&image);

    // Read the raw image from file
    // send row by row
    FILE *fp = fopen(FILE_IN_NAME, "rb");
    int8_t img_row[W] = {0};
    unsigned ta = 0, tb = 0; 
    for (int i = 0; i < image.height; i++) {
        fread((uint8_t*)&img_row[0], 1, image.width, fp);
        ta = get_reference_time();
        CAMERA_ISP_RAW8_RGB1(&image, img_row, i);
        tb += get_reference_time() - ta;
    }
    float ops_per_pixel = (float)tb / (image.height * image.width * image.channels);
    printf("Average time per row (ms): %f\n", TO_MS(tb / image.height));
    printf("Total time (ms): %f\n", TO_MS(tb));
    printf("Ops per pixel: %.2f\n", ops_per_pixel);
    fclose(fp);

    // Write the image to file
    printf("Writing image to file\n");
    uint8_t *img_ptr = (uint8_t*)image.ptr;
    camera_io_write_file(FILE_OUT_NAME, img_ptr, image.size);
}

int main(){
    test_isp();
    return 0;
}

// python ../../python/run_xscope_bin.py bin/test_isp_rgb1.xe
