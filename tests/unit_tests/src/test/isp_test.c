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

TEST_GROUP_RUNNER(isp_tests) {
  RUN_TEST_CASE(isp_tests, yuv_to_rgb);
  RUN_TEST_CASE(isp_tests, rgb_to_yuv);
}

TEST_GROUP(isp_tests);
TEST_SETUP(isp_tests) { fflush(stdout); }
TEST_TEAR_DOWN(isp_tests) {}


#define INV_DELTA 30  // error allowed in YUV RGB color conversion 
#define CT_INT    127 // int conversion

// Store the RGB color and corresponding values
typedef struct
{
    int R, G, B;
    int Y, U, V;
} color_table_t;


void printColorTable(color_table_t* table) {
    printf("Color Table:\n");
    printf("R: %d, G: %d, B: %d\n", table->R, table->G, table->B);
    printf("Y: %d, U: %d, V: %d\n", table->Y, table->U, table->V);
}

void yuv_to_rgb_ct(color_table_t* ct_ref, color_table_t* ct_res){
    *ct_res = *ct_ref;
    uint32_t result = yuv_to_rgb(ct_ref->Y - CT_INT, ct_ref->U - CT_INT, ct_ref->V - CT_INT);
    ct_res -> R =  (uint8_t)(GET_R(result) + CT_INT);
    ct_res -> G =  (uint8_t)(GET_G(result) + CT_INT);
    ct_res -> B =  (uint8_t)(GET_B(result) + CT_INT);
}

void rgb_to_yuv_ct(color_table_t* ct_ref, color_table_t* ct_res){
    *ct_res = *ct_ref;
    uint32_t result = rgb_to_yuv(ct_ref->Y - CT_INT, ct_ref->U - CT_INT, ct_ref->V - CT_INT);
    ct_res -> Y =  (uint8_t)(GET_Y(result) + CT_INT);
    ct_res -> U =  (uint8_t)(GET_U(result) + CT_INT);
    ct_res -> V =  (uint8_t)(GET_V(result) + CT_INT);
}



color_table_t ct_test = {16, 100, 16, 65, 100, 92}; // R G B Y U V 
//TODO include more colors for testing

TEST(isp_tests, yuv_to_rgb)
{
  // Define color table 
  color_table_t ct_ref = ct_test;
  color_table_t ct_result;

  // Test the converison
  yuv_to_rgb_ct(&ct_ref, &ct_result);

  printColorTable(&ct_test);
  printColorTable(&ct_result);

  // Ensure conversion is correct
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.R, ct_result.R);
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.G, ct_result.G);
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.B, ct_result.B);
}


TEST(isp_tests, rgb_to_yuv)
{
  // Define color table 
  color_table_t ct_ref = ct_test;
  color_table_t ct_result;

  // Test the converison
  rgb_to_yuv_ct(&ct_ref, &ct_result);

  printColorTable(&ct_test);
  printColorTable(&ct_result);

  // Printing the extracted bytes
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.Y, ct_result.Y);
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.U, ct_result.U);
  TEST_ASSERT_INT_WITHIN(INV_DELTA, ct_ref.V, ct_result.V);
}
