// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>

#include <print.h>
#include <xcore/assert.h>

#include "camera_conv.h"

static
void dot_product(const int16_t T[3][4], int16_t x, int16_t y, int16_t z, int16_t out[3]) {
    out[0] = T[0][0] * x + T[0][1] * y + T[0][2] * z;
    out[1] = T[1][0] * x + T[1][1] * y + T[1][2] * z;
    //out[2] = T[2][0] * x + T[2][1] * y + T[2][2] * z; // not needed
}

void camera_swap_dims(uint8_t* image_in, uint8_t* image_out, const unsigned height, const unsigned width, const unsigned channels) {
    printstrln("Swapping image dimensions...");
    for (unsigned k = 0; k < height; k++) {
        for (unsigned j = 0; j < width; j++) {
            for (unsigned c = 0; c < channels; c++) {
                unsigned index_in = c * (height * width) + k * width + j;
                unsigned index_out = k * (width * channels) + j * channels + c;
                image_out[index_out] = image_in[index_in];
            }
        }
    }
    printstrln("Done.");
}

void camera_rotate90(void* dst_img, void* src_img, const int16_t h, const int16_t w, const int16_t ch) {
    xassert((w > 0) && (h > 0) && (ch > 0) && "Width, height and channels must be positive");
    xassert((w % 2 == 0) && (h % 2 == 0) && "Width and height must be even");

    // Define T
    const int16_t T[3][4] = {
        {0, -1, (h - 1), 0},
        {1, 0, 0, 0},
        {0, 0, 1, 0}
    };

    // Iterate
    int16_t x = 0, y = 0;
    int16_t xp = 0, yp = 0;
    int16_t pix_out[3] = { 0 };

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            // Compute the dot product
            dot_product(T, x, y, 1, pix_out);
            xp = pix_out[0];
            yp = pix_out[1];
            // Copy from location (y, x) to location (yp, xp)
            uint8_t* src_pixel = (uint8_t*)&src_img[y * w * ch + x * ch];
            uint8_t* dst_pixel = (uint8_t*)&dst_img[yp * h * ch + xp * ch];
            dst_pixel[0] = src_pixel[0];
            dst_pixel[1] = src_pixel[1];
            dst_pixel[2] = src_pixel[2];
        }
    }
}
