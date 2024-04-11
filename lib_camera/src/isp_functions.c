// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <string.h>

#include <xcore/assert.h>

#include "isp_functions.h"

// -------------------------------- Crop  --------------------------------

void isp_crop(
  void* img,
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

void isp_crop_int8(
  int8_t* img,
  const unsigned in_width,
  const unsigned in_height,
  unsigned xu1,
  unsigned yu1,
  unsigned xu2,
  unsigned yu2) {
  isp_crop(img, in_width, in_height, xu1, yu1, xu2, yu2);
}

void isp_crop_uint8(
  uint8_t* img,
  const unsigned in_width,
  const unsigned in_height,
  unsigned xu1,
  unsigned yu1,
  unsigned xu2,
  unsigned yu2) {
  isp_crop(img, in_width, in_height, xu1, yu1, xu2, yu2);
}


// -------------------------------- Resize  --------------------------------

static inline float unsigned_to_float(const unsigned val) {
  // Note: if the word has more than 23 consecutive bits, data will be lost
  int32_t exp = 23, zero = 0;
  float res;
  asm("fmake %0, %1, %2, %3, %4" : "=r"(res) : "r"(zero), "r"(exp), "r"(zero), "r"(val));
  return res;
}

static inline void xmodf(float a, unsigned* b, float* c, unsigned* bp) {
  int32_t zero = 0, tmp, exp; // tmp is used to mask the mantissa
  unsigned mant;
  asm("fsexp %0, %1, %2": "=r" (zero), "=r" (exp) : "r" (a));
  asm("fmant %0, %1": "=r" (mant) : "r" (a));
  exp -= 23;
  *b = mant >> -exp;
  asm("mkmsk %0, %1": "=r" (tmp) : "r" (-exp));
  mant &= tmp;
  exp += 23;
  asm("fmake %0, %1, %2, %3, %4": "=r" (*c) : "r" (zero), "r" (exp), "r" (zero), "r" (mant));
  *bp = *b + 1;
}

static inline uint32_t float_to_uq23(const float val) {
  // this assumes that the input [0.0, 1.0)
  int32_t zero, exp; uint32_t mant;
  asm("fsexp %0, %1, %2" : "=r"(zero), "=r"(exp) : "r"(val));
  asm("fmant %0, %1" : "=r"(mant) : "r"(val));
  mant <<= exp;
  return mant;
}

void isp_resize_uint8(
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
  uint32_t a, b, c, d;

  for (unsigned i = 0; i < out_height; i++) {
    float incry = (y_ratio * unsigned_to_float(i));
    xmodf(incry, &y_l, &yw, &y_h);

    for (unsigned j = 0; j < out_width; j++) {
      float incrx = (x_ratio * unsigned_to_float(j));
      xmodf(incrx, &x_l, &xw, &x_h);

      float xyw = xw * yw;
      float yw_m_xyw = yw - xyw;
      uint32_t W = float_to_uq23(1 - xw - yw_m_xyw);
      uint32_t X = float_to_uq23(xw - xyw);
      uint32_t Y = float_to_uq23(yw_m_xyw);
      uint32_t Z = float_to_uq23(xyw);

      for (unsigned plane = 0; plane < 3; plane++) {
        a = img[3 * in_width * y_l + 3 * x_l + plane];
        b = img[3 * in_width * y_l + 3 * x_h + plane];
        c = img[3 * in_width * y_h + 3 * x_l + plane];
        d = img[3 * in_width * y_h + 3 * x_h + plane];

        uint32_t ah = 0, al = 0;
        asm("maccu %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (a), "r" (W), "0" (ah), "1" (al));
        asm("maccu %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (b), "r" (X), "0" (ah), "1" (al));
        asm("maccu %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (c), "r" (Y), "0" (ah), "1" (al));
        asm("maccu %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (d), "r" (Z), "0" (ah), "1" (al));
        // assumes that a * W + b * X + c * Y + d * Z never overflows uint8
        uint8_t pixel = (uint8_t)(al >> 23);
        out_img[3 * out_width * i + 3 * j + plane] = pixel;
      }
    }
  }
}

static inline int32_t float_to_q23(const float val) {
  // this assumes that the input [-1.0, 1.0)
  int32_t sign, exp, mant;
  asm("fsexp %0, %1, %2" : "=r"(sign), "=r"(exp) : "r"(val));
  asm("fmant %0, %1" : "=r"(mant) : "r"(val));
  if(sign){mant = -mant;}
  mant <<= exp;
  return mant;
}

void isp_resize_int8(
  const int8_t* img,
  const unsigned in_width,
  const unsigned in_height,
  int8_t* out_img,
  const unsigned out_width,
  const unsigned out_height) {
  const float x_ratio = ((unsigned_to_float(in_width) - 1) / (unsigned_to_float(out_width) - 1));
  const float y_ratio = ((unsigned_to_float(in_height) - 1) / (unsigned_to_float(out_height) - 1));

  unsigned x_l, y_l, x_h, y_h;
  float xw, yw;
  int32_t a, b, c, d;

  for (unsigned i = 0; i < out_height; i++) {
    float incry = (y_ratio * unsigned_to_float(i));
    xmodf(incry, &y_l, &yw, &y_h);

    for (unsigned j = 0; j < out_width; j++) {
      float incrx = (x_ratio * unsigned_to_float(j));
      xmodf(incrx, &x_l, &xw, &x_h);

      float xyw = xw * yw;
      float yw_m_xyw = yw - xyw;
      int32_t W = float_to_q23(1 - xw - yw_m_xyw);
      int32_t X = float_to_q23(xw - xyw);
      int32_t Y = float_to_q23(yw_m_xyw);
      int32_t Z = float_to_q23(xyw);
      
      for (unsigned plane = 0; plane < 3; plane++) {
        a = (int32_t)img[3 * in_width * y_l + 3 * x_l + plane];
        b = (int32_t)img[3 * in_width * y_l + 3 * x_h + plane];
        c = (int32_t)img[3 * in_width * y_h + 3 * x_l + plane];
        d = (int32_t)img[3 * in_width * y_h + 3 * x_h + plane];

        int32_t ah = 0, al = 0;
        asm("maccs %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (a), "r" (W), "0" (ah), "1" (al));
        asm("maccs %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (b), "r" (X), "0" (ah), "1" (al));
        asm("maccs %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (c), "r" (Y), "0" (ah), "1" (al));
        asm("maccs %0, %1, %2, %3": "=r" (ah), "=r" (al): "r" (d), "r" (Z), "0" (ah), "1" (al));
        // assumes that a * W + b * X + c * Y + d * Z never overflows int8
        int8_t pixel = (int8_t)(al >> 23);

        out_img[3 * out_width * i + 3 * j + plane] = pixel;
      }
    }
  }
}
