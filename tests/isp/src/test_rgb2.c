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

#define INPUT_FILE  "imgs/in_rgb2.bin" 
#define OUTPUT_FILE "imgs/out_rgb2.rgb" 

// Input image configuration
#define in_h 200
#define in_w 200
#define in_ch 1
#define in_size in_h * in_w * in_ch

// Output image configuration
#define out_h  100
#define out_w  100
#define out_ch 3
#define out_size out_h * out_w * out_ch

void test_isp_rgb2() {    
    // Time variables
    unsigned ta = 0, tb = 0;  
    
    // Create a Configuration
    int8_t image_buffer[out_size] ALIGNED_8 = { 0 };

    camera_cfg_t config = {
        .offset_x = 0,
        .offset_y = 0,
        .mode = MODE_RGB2,
    };
    image_cfg_t image = {
        .height = out_h,
        .width = out_w,
        .channels = out_ch,
        .size = out_size,
        .ptr = &image_buffer[0],
        .config = &config
    };
    camera_isp_coordinates_compute(&image);

    // Read the raw image from file
    // send it row by row
    FILE *fp = fopen(INPUT_FILE, "rb");
    assert(fp != NULL);
    int8_t img_row[in_w] = {0}; // aux buffer for reading the image
    for (int i = 0; i < in_h; i++) {
        fread((uint8_t*)&img_row[0], 1, in_w, fp);        
        ta = get_reference_time();
        camera_isp_raw8_to_rgb2(&image, img_row, i);
        tb += get_reference_time() - ta;
    }
    float ops_per_pixel = (float)tb / image.size;
    printf("Average time per row (ms): %f\n", TO_MS(tb / image.height));
    printf("Total time (ms): %f\n", TO_MS(tb));
    printf("Ops per pixel: %.2f\n", ops_per_pixel);
    fclose(fp);

    // Write the image to file
    printf("Writing image to file\n");
    camera_io_write_file(OUTPUT_FILE, (uint8_t*)image.ptr, image.size);
}

int main(){
    printf("[test_isp_rgb2]\n");
    test_isp_rgb2();
    return 0;
}
