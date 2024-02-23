// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>

#include "isp_functions.hpp"

// -------------------------------- Crop  --------------------------------
template<typename T>
void isp_crop_template(
    T* img,
    const unsigned in_width,
    const unsigned in_height,
    unsigned xu1,
    unsigned yu1,
    unsigned xu2,
    unsigned yu2) {

    xu2 = (xu2 >= in_width) ? in_width - 1 : xu2;

    unsigned out_width = xu2 - xu1;
    xassert(out_width <= in_width);
    xassert((yu2 - yu1) <= in_height);
    unsigned in_row_len = in_width * 3;

    unsigned out_index = 0;
    unsigned in_index = yu1 * in_row_len + xu1 * 3;
    unsigned out_row_len = out_width * 3;

    for (unsigned i = yu1; i < yu2; i++) {
        memmove(&img[out_index], &img[in_index], out_row_len);
        in_index += in_row_len;
        out_index += out_row_len;
    }
}

// -------------------------------- Resize  --------------------------------
static void xmodf(float a, unsigned* b, float* c, unsigned* bp)
{
    // split unsignedeger and decimal part
    *b = (unsigned)(a);
    *c = a - *b;
    // last operand for convinience 
    *bp = *b + 1;
}
static inline float uint8_to_float(const uint8_t val) {
    int32_t mant = val, exp = 23, zero = 0;
    float res;
    asm("fmake %0, %1, %2, %3, %4" : "=r"(res) : "r"(zero), "r"(exp), "r"(zero), "r"(mant));
    return res;
}
static inline float unsigned_to_float(const unsigned val) {
    // will lose some data with long words
    int32_t exp = 23, zero = 0;
    float res;
    asm("fmake %0, %1, %2, %3, %4" : "=r"(res) : "r"(zero), "r"(exp), "r"(zero), "r"(val));
    return res;
}
static inline uint8_t float_to_uint8(const float val) {
    // this assumes that the input [0.0, 255.0]
    int32_t zero, exp, mant;
    asm("fsexp %0, %1, %2" : "=r"(zero), "=r"(exp) : "r"(val));
    asm("fmant %0, %1" : "=r"(mant) : "r"(val));
    exp -= 23; // because
    mant >>= -exp;
    return (uint8_t)(mant & 0xff);
}

/*
static inline unsigned float_to_unsigned(const float val) { // not used
    int32_t zero, exp, mant;
    asm("fsexp %0, %1, %2" : "=r"(zero), "=r"(exp) : "r"(val));
    asm("fmant %0, %1" : "=r"(mant) : "r"(val));
    exp -= 23; // because
    mant >>= -exp;
    // saturate
    if (exp > 8) {
        mant = UINT32_MAX;
    }
    return mant;
}
*/



template<typename T>
void isp_resize_template(
    const T* img,
    const unsigned in_width,
    const unsigned in_height,
    T* out_img,
    const unsigned out_width,
    const unsigned out_height) {
    const float x_ratio = ((in_width - 1) / (float)(out_width - 1));
    const float y_ratio = ((in_height - 1) / (float)(out_height - 1));

    unsigned x_l, y_l, x_h, y_h;
    float xw, yw;
    T a, b, c, d;

    for (unsigned i = 0; i < out_height; i++) {
        for (unsigned j = 0; j < out_width; j++) {
            float incrx = (x_ratio * j);
            float incry = (y_ratio * i);

            xmodf(incrx, &x_l, &xw, &x_h);
            xmodf(incry, &y_l, &yw, &y_h);

            for (unsigned plane = 0; plane < 3; plane++) {
                a = img[3 * in_width * y_l + 3 * x_l + plane];
                b = img[3 * in_width * y_l + 3 * x_h + plane];
                c = img[3 * in_width * y_h + 3 * x_l + plane];
                d = img[3 * in_width * y_h + 3 * x_h + plane];

                T pixel = (T)(a * (1 - xw) * (1 - yw) +
                    b * xw * (1 - yw) +
                    c * yw * (1 - xw) +
                    d * xw * yw);

                out_img[3 * out_width * i + 3 * j + plane] = pixel;
            }
        }
    }
}



void isp_resize_uint8_opt(
    const uint8_t* img,
    const unsigned in_width,
    const unsigned in_height,
    uint8_t* out_img,
    const unsigned out_width,
    const unsigned out_height) 
{
    const float x_ratio = (unsigned_to_float(in_width - 1) / unsigned_to_float(out_width - 1));
    const float y_ratio = (unsigned_to_float(in_height - 1) / unsigned_to_float(out_height - 1));

    unsigned x_l, y_l, x_h, y_h;
    float xw, yw;
    uint8_t a, b, c, d;

    for (unsigned i = 0; i < out_height; i++) {
        float incry = (y_ratio * unsigned_to_float(i));
        xmodf(incry, &y_l, &yw, &y_h);
        float yw_inv = (1 - yw);

        for (unsigned j = 0; j < out_width; j++) {
            float incrx = (x_ratio * unsigned_to_float(j));
            xmodf(incrx, &x_l, &xw, &x_h);
            float xw_inv = (1 - xw);
            float W = xw_inv * yw_inv;
            float X = xw * yw_inv;
            float Y = yw * xw_inv;
            float Z = xw * yw;

            for (unsigned plane = 0; plane < 3; plane++) {
                a = img[3 * in_width * y_l + 3 * x_l + plane];
                b = img[3 * in_width * y_l + 3 * x_h + plane];
                c = img[3 * in_width * y_h + 3 * x_l + plane];
                d = img[3 * in_width * y_h + 3 * x_h + plane];
                // can be optimised with fmacc
                uint8_t pixel = float_to_uint8(uint8_to_float(a) * W + uint8_to_float(b) * X + uint8_to_float(c) * Y + uint8_to_float(d) * Z);
                out_img[3 * out_width * i + 3 * j + plane] = pixel;
            }
        }
    }
}

// -------------------------------- C Wrapper functions  --------------------------------

// Extern C
void isp_crop_int8(
    int8_t* img,
    const unsigned in_width,
    const unsigned in_height,
    unsigned xu1,
    unsigned yu1,
    unsigned xu2,
    unsigned yu2) {
    isp_crop_template(img, in_width, in_height, xu1, yu1, xu2, yu2);
}

// Extern C
void isp_crop_uint8(
    uint8_t* img,
    const unsigned in_width,
    const unsigned in_height,
    unsigned xu1,
    unsigned yu1,
    unsigned xu2,
    unsigned yu2) {
    isp_crop_template(img, in_width, in_height, xu1, yu1, xu2, yu2);
}

// Extern C
void isp_resize_int8(
    const int8_t* img,
    const unsigned in_width,
    const unsigned in_height,
    int8_t* out_img,
    const unsigned out_width,
    const unsigned out_height) {
    isp_resize_template(img, in_width, in_height, out_img, out_width, out_height);
}

// Extern C
void isp_resize_uint8(
    const uint8_t* img,
    const unsigned in_width,
    const unsigned in_height,
    uint8_t* out_img,
    const unsigned out_width,
    const unsigned out_height) {
    isp_resize_template(img, in_width, in_height, out_img, out_width, out_height);
}
