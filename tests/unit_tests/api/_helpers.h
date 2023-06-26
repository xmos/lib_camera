#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

// Print formatting
#define print_separator(x) printf("\n---------- %s -------------\n", x)

// Common difinitions
#define CT_INT    127 // int conversion

// Store the RGB color and corresponding values
typedef struct
{
    uint8_t R, G, B;
    uint8_t Y, U, V;
} color_table_t;

typedef enum{
    RGB_TO_YUV,
    YUV_TO_RGB
} color_conversion_t;


// Fill array
void fill_array_rand_int8(int8_t *image, size_t size);
void fill_array_rand_uint8(uint8_t *image, size_t size);

void fill_color_table_uint8(color_table_t table[], const size_t size, color_conversion_t conversion);
void printColorTable(color_table_t *table, uint8_t ref);
void yuv_to_rgb_ct(color_table_t *ct_ref, color_table_t *ct_res);
void rgb_to_yuv_ct(color_table_t *ct_ref, color_table_t *ct_res);
