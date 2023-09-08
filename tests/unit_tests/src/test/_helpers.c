// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <time.h>
#include <assert.h>

#include "_helpers.h"
#include "camera_main.h"

// Random generators
int8_t generate_rand_int8(){
  return (rand() % 256) - 128;
}
uint8_t generate_rand_uint8(){
  return (rand() % 256);
}

// Single array fill functions
void fill_array_rand_uint8(uint8_t *image, const size_t size){
  for(size_t idx = 0; idx < size; idx++){
    image[idx] = generate_rand_uint8();
  }
}

void fill_array_rand_int8(int8_t *image, const size_t size){
  for(size_t idx = 0; idx < size; idx++){
    image[idx] = generate_rand_int8();
  }
}


// Color conversion functions
YuvValues rgbToYuv(uint8_t r, uint8_t g, uint8_t b) {
  float fr = (float)r;
  float fg = (float)g;
  float fb = (float)b;

  YuvValues yuv;
  yuv.y = (uint8_t)(0.299f * fr + 0.587f * fg + 0.114f * fb + 0.5f);
  yuv.u = (uint8_t)(-0.168736 * fr -0.331264 * fg + 0.500000 * fb + 128.0f);
  yuv.v = (uint8_t)(0.500000 * fr -0.418688 * fg -0.081312 * fb + 128.0f);

  return yuv;
}

RgbValues yuvToRgb(uint8_t y, uint8_t u, uint8_t v) {
  RgbValues rgb;

  float fy = (float)y;
  float fu = (float)(u - 128);
  float fv = (float)(v - 128);

  float red = fy + 1.1406f  * fv;
  float green = fy - 0.3960f * fu - 0.5843f * fv;
  float blue = fy + 2.0392f  * fu;

  rgb.r = (uint8_t)(red > 255.0f ? 255 : (red < 0.0f ? 0 : red));
  rgb.g = (uint8_t)(green > 255.0f ? 255 : (green < 0.0f ? 0 : green));
  rgb.b = (uint8_t)(blue > 255.0f ? 255 : (blue < 0.0f ? 0 : blue));

  return rgb;
}

void fill_color_table_uint8(color_table_t table[], const size_t size, color_conversion_t conversion){
  if (conversion == RGB_TO_YUV){
    for(size_t idx = 0; idx < size; idx++){
        table[idx].R = generate_rand_uint8();
        table[idx].G = generate_rand_uint8();
        table[idx].B = generate_rand_uint8();
        YuvValues yuv = rgbToYuv(table[idx].R, table[idx].G, table[idx].B);
        table[idx].Y =  yuv.y;
        table[idx].U =  yuv.u;
        table[idx].V =  yuv.v;
      }
  }
  else if (conversion == YUV_TO_RGB){
    for(size_t idx = 0; idx < size; idx++){
        table[idx].Y = generate_rand_uint8();
        table[idx].U = generate_rand_uint8();
        table[idx].V = generate_rand_uint8();
        RgbValues rgb = yuvToRgb(table[idx].Y, table[idx].U, table[idx].V);
        table[idx].R =  rgb.r;
        table[idx].G =  rgb.g;
        table[idx].B =  rgb.b;
      }
  }
  else{
    printf("Invalid color conversion type\n");
    assert(0);
  }

  
}

void printColorTable(color_table_t* table, uint8_t ref) {
  if(ref)
  {
    printf("Expected ");
  }
  else
  {
    printf("Resulted ");
  }
  printf("Color Table:\n");
  printf("R: %d, G: %d, B: %d\n", table->R, table->G, table->B);
  printf("Y: %d, U: %d, V: %d\n", table->Y, table->U, table->V);
}

void yuv_to_rgb_ct(color_table_t* ct_ref, color_table_t* ct_res){
  ct_res -> Y = ct_ref -> Y;
  ct_res -> U = ct_ref -> U;
  ct_res -> V = ct_ref -> V;
  uint32_t result = yuv_to_rgb(ct_ref->Y - CT_INT, ct_ref->U - CT_INT, ct_ref->V - CT_INT);
  ct_res -> R =  (uint8_t)(GET_R(result) + CT_INT);
  ct_res -> G =  (uint8_t)(GET_G(result) + CT_INT);
  ct_res -> B =  (uint8_t)(GET_B(result) + CT_INT);
}

void rgb_to_yuv_ct(color_table_t* ct_ref, color_table_t* ct_res){
  ct_res -> R = ct_ref -> R;
  ct_res -> G = ct_ref -> G;
  ct_res -> B = ct_ref -> B;
  uint32_t result = rgb_to_yuv(ct_ref->R - CT_INT, ct_ref->G - CT_INT, ct_ref->B - CT_INT);
  ct_res -> Y =  (uint8_t)(GET_Y(result) + CT_INT);
  ct_res -> U =  (uint8_t)(GET_U(result) + CT_INT);
  ct_res -> V =  (uint8_t)(GET_V(result) + CT_INT);
}
