// Copyright 2020-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera.h"

TEST_GROUP_RUNNER(image_hfilter) {
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__basic);
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__acc_init);
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__apply_shift);
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__input_stride);
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__out_count);
  RUN_TEST_CASE(image_hfilter, pixel_hfilter__alt_coef);

  RUN_TEST_CASE(image_hfilter, image_hfilter_update_scale__case1);
  RUN_TEST_CASE(image_hfilter, image_hfilter_update_scale__case2);
  RUN_TEST_CASE(image_hfilter, image_hfilter_update_scale__case3);
  RUN_TEST_CASE(image_hfilter, image_hfilter_update_scale__case4);

  // RUN_TEST_CASE(image_hfilter, image_hfilter__case1);
}

TEST_GROUP(image_hfilter);
TEST_SETUP(image_hfilter) { fflush(stdout); }
TEST_TEAR_DOWN(image_hfilter) {}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__basic)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 16;
  static const int32_t input_stride = 4;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 1;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};

  for(int k = 0; k < input_count; k++) 
    input[k] = k>>2;
  
  const int32_t acc_init = 0;
  const unsigned shift = 0;

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);

  for(int k = 0; k < output_count; k++)   
    TEST_ASSERT_EQUAL_INT8(k, output[k]);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__acc_init)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 16;
  static const int32_t input_stride = 4;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 1;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};

  for(int k = 0; k < input_count; k++)  
    input[k] = k>>2;
  
  const int32_t acc_init = 10;
  const unsigned shift = 0;

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);


  for(int k = 0; k < output_count; k++)   
    TEST_ASSERT_EQUAL_INT8(acc_init+k, output[k]);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__apply_shift)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 16;
  static const int32_t input_stride = 4;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 1;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};

  for(int k = 0; k < input_count; k++)  
    input[k] = k;
  
  const int32_t acc_init = 32;
  const unsigned shift = 2;

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);

  for(int k = 0; k < output_count; k++)   
    TEST_ASSERT_EQUAL_INT8( (acc_init>>2)+k, 
                            output[k] );
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__input_stride)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 16;
  static const int32_t input_stride = 8;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 1;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};

  for(int k = 0; k < input_count; k++)  
    input[k] = k>>2;
  
  const int32_t acc_init = 10;
  const unsigned shift = 0;

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);

  for(int k = 0; k < output_count; k++)   
    TEST_ASSERT_EQUAL_INT8(acc_init+2*k, output[k]);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__out_count)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 64;
  static const int32_t input_stride = 4;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 1;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};

  for(int k = 0; k < input_count; k++)  
    input[k] = k>>2;
  
  const int32_t acc_init = 10;
  const unsigned shift = 0;

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);

  for(int k = 0; k < output_count; k++)   
    TEST_ASSERT_EQUAL_INT8(acc_init+k, output[k]);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, pixel_hfilter__alt_coef)
{
  static const int8_t coef_count = 32;
  static const unsigned output_count = 32;
  static const int32_t input_stride = 4;
  static const unsigned input_count = 32 + (output_count-1)*input_stride;

  int8_t coef[coef_count] = {0};
  coef[0] = 2;
  coef[1] = 4;

  int8_t input[input_count] = {0};
  int8_t output[output_count] = {0};
  int8_t expected[output_count] = {0};

  for(int k = 0; k < input_count; k++)  
    input[k] = k >> 2;
  
  const int32_t acc_init = 16;
  const unsigned shift = 0;

  for(int k = 0; k < output_count; k++){
    int expected_val = acc_init;
    for(int j = 0; j < coef_count; j++)
      expected_val += coef[j] * input[k*input_stride + j];
    
    expected[k] = (expected_val >= INT8_MAX) ? INT8_MAX 
                : (expected_val <= INT8_MIN) ? INT8_MIN
                : expected_val;
  }

  pixel_hfilter(output, input, coef, 
                acc_init, shift, input_stride, output_count);

  TEST_ASSERT_EQUAL_INT8_ARRAY(expected, output, output_count);

}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, image_hfilter_update_scale__case1)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 0.0f;

  image_hfilter_update_scale(&state, gain, 0);

  const unsigned exp_shift = 9;
  const int32_t exp_acc_init = -128 * (1<<exp_shift);

  TEST_ASSERT_EQUAL_INT8(0, state.coef[0]);
  TEST_ASSERT_EQUAL_INT8(0, state.coef[2]);
  TEST_ASSERT_EQUAL_INT8(0, state.coef[4]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, image_hfilter_update_scale__case2)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 1.0f;

  image_hfilter_update_scale(&state, gain, 1);

  const unsigned exp_shift = 7;
  const int32_t exp_acc_init = 0;

  TEST_ASSERT_EQUAL_INT8(0x1B, state.coef[1]);
  TEST_ASSERT_EQUAL_INT8(0x4B, state.coef[3]);
  TEST_ASSERT_EQUAL_INT8(0x1B, state.coef[5]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, image_hfilter_update_scale__case3)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 1.2f;

  image_hfilter_update_scale(&state, gain, 0);

  const unsigned exp_shift = 7;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * (1<<exp_shift);

  TEST_ASSERT_EQUAL_INT8(0x20, state.coef[0]);
  TEST_ASSERT_EQUAL_INT8(0x59, state.coef[2]);
  TEST_ASSERT_EQUAL_INT8(0x20, state.coef[4]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(image_hfilter, image_hfilter_update_scale__case4)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 0.8f;

  image_hfilter_update_scale(&state, gain, 1);

  const unsigned exp_shift = 8;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * (1<<exp_shift);

  TEST_ASSERT_EQUAL_INT8(0x2B, state.coef[1]);
  TEST_ASSERT_EQUAL_INT8(0x77, state.coef[3]);
  TEST_ASSERT_EQUAL_INT8(0x2B, state.coef[5]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}


// ///////////////////////////////////////////////
// ///////////////////////////////////////////////
// ///////////////////////////////////////////////
// TEST(image_hfilter, image_hfilter__case1)
// {
//   static const unsigned output_count = 16;
//   static const int32_t input_stride = 4;
//   static const unsigned input_count = 32 + (output_count-1)*input_stride;

//   hfilter_state_t state;
//   memset(&state, 0, sizeof(state));

//   // 1.0f should give coefficients  0x1B, 0x4B, 0x1B
//   image_hfilter_update_scale(&state, 1.0f, 0);

//   int8_t input[input_count] = {0};
//   int8_t output[output_count] = {0};
//   int8_t expected[output_count] = {0};

//   for(int k = 0; k < input_count; k++) 
//     input[k] = k;

//   image_hfilter(output, &state, input(


//   pixel_hfilter(output, input, coef, 
//                 acc_init, shift, input_stride, output_count);

//   for(int k = 0; k < output_count; k++)   
//     TEST_ASSERT_EQUAL_INT8(k, output[k]);
// }
