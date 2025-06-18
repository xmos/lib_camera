// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Decimator factor for tests
#define K 4 // downsampled image / 4*4

// Print formatting
#define print_separator(x) printf("\n---------- %s -------------\n", x)

// Common difinitions
#define CT_INT 127 // int conversion


#pragma once

#include <stdio.h>
#include <print.h>
#include <xcore/hwtimer.h>

#define TO_MS(ticks) ((float)(ticks) / XS1_TIMER_KHZ)
#define PRINT_STR_MSG ">> ticks: "

#define TIMEIT(func, ...) \
    { \
        unsigned start_time = get_reference_time(); \
        func(__VA_ARGS__); \
        unsigned end_time = get_reference_time(); \
        printstr(PRINT_STR_MSG); \
        printuintln(end_time - start_time); \
    }

// --------------------------- Image creation -----------------------------------------
#define CREATE_IMG_UINT8(name, h, w, c)      \
struct name {                                \
    uint8_t data[h][w][c];                   \
    uint8_t* ptr;                            \
    const uint16_t width;                    \
    const uint16_t height;                   \
    const uint16_t channels;                 \
    const unsigned size;                     \
} name = {                                   \
    .ptr = (uint8_t*)&name.data[0][0][0],    \
    .width = w,                              \
    .height = h,                             \
    .channels = c,                           \
    .size = w * h * c                        \
}                             

#define CREATE_IMG_INT8(name, h, w, c)       \
struct name {                                \
    int8_t data[h][w][c];                    \
    int8_t* ptr;                             \
    const uint16_t width;                    \
    const uint16_t height;                   \
    const uint16_t channels;                 \
    const unsigned size;                     \
} name = {                                   \
    .ptr = (int8_t*)&name.data[0][0][0],     \
    .width = w,                              \
    .height = h,                             \
    .channels = c,                           \
    .size = w * h * c                        \
} 

// Print macros
// whatever name, number of cycles measured, number of pixels processed, number of bytes in pixel
#define PRINT_TIME(name, cycles, n_pix, bpp) \
printf("|    %5.3f\t  |   %6.3f\t     | %s\n", (float)cycles / n_pix / bpp, (float)cycles / n_pix, name);

/**
 * Measure the cpu ticks
 * 
 * @return ticks - Number of ticks
 */
unsigned measure_time();

// Store the RGB color and corresponding values
typedef struct
{
  uint8_t R, G, B;
  uint8_t Y, U, V;
} color_table_t;

typedef enum
{
  RGB_TO_YUV,
  YUV_TO_RGB
} color_conversion_t;

typedef struct
{
  uint8_t y;
  uint8_t u;
  uint8_t v;
} YuvValues;

typedef struct
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} RgbValues;

// -----------------------------------------------------
void fill_array_rand_int8(int8_t *image, size_t size);
void fill_array_rand_uint8(uint8_t *image, size_t size);
void print_array(int8_t* result, size_t size);

void fill_color_table_uint8(color_table_t table[], const size_t size, color_conversion_t conversion);
void printColorTable(color_table_t *table, uint8_t ref);
void yuv_to_rgb_ct(color_table_t *ct_ref, color_table_t *ct_res);
void rgb_to_yuv_ct(color_table_t *ct_ref, color_table_t *ct_res);

void rgb_to_greyscale_float(int8_t * gs_img, int8_t * img, unsigned n_pix);
RgbValues yuvToRgb(uint8_t y, uint8_t u, uint8_t v);
YuvValues rgbToYuv(uint8_t r, uint8_t g, uint8_t b);
