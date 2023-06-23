// Copyright 2020-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera_main.h"

#define print_separator(x) printf("\n---------- %s -------------\n", x)

TEST_GROUP_RUNNER(color_conversion) {
  RUN_TEST_CASE(color_conversion, yuv_to_rgb);
  RUN_TEST_CASE(color_conversion, rgb_to_yuv);
}

TEST_GROUP(color_conversion);
TEST_SETUP(color_conversion) { fflush(stdout); print_separator("color_conversion");}
TEST_TEAR_DOWN(color_conversion) {}


#define INV_DELTA 20  // error allowed in YUV RGB color conversion 
#define CT_INT    127 // int conversion

// Store the RGB color and corresponding values
typedef struct
{
    int R, G, B;
    int Y, U, V;
} color_table_t;

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

// R G B Y U V
#define num_tests 3
color_table_t ct_test_vector[num_tests] = {
          {48, 100, 16, 74, 94, 108},
          {192, 70, 23, 101, 83, 192},
          {58, 3, 156, 36, 195, 143}
};
//TODO randomize this data

TEST(color_conversion, yuv_to_rgb)
{
  for(size_t i = 0; i < num_tests; i++)
  {
    // Define color table 
    color_table_t ct_ref = ct_test_vector[i];
    color_table_t ct_result = {0};

    // Test the converison
    yuv_to_rgb_ct(&ct_ref, &ct_result);

    printColorTable(&ct_ref, 1);
    printColorTable(&ct_result, 0);

    // Ensure conversion is correct
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.R, ct_result.R);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.G, ct_result.G);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.B, ct_result.B);
  }
}

TEST(color_conversion, rgb_to_yuv)
{
  for(size_t i = 0; i < num_tests; i++)
  {
    // Define color table 
    color_table_t ct_ref = ct_test_vector[i];
    color_table_t ct_result = {0};

    // Test the converison
    rgb_to_yuv_ct(&ct_ref, &ct_result);

    printColorTable(&ct_ref, 1);
    printColorTable(&ct_result, 0);

    // Printing the extracted bytes
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.Y, ct_result.Y);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.U, ct_result.U);
    TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.V, ct_result.V);
  }
}
