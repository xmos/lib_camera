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
#include "camera_utils.h"

TEST_GROUP_RUNNER(pixel_hfilter) {
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__basic);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__acc_init);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__apply_shift);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__input_stride);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__out_count);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__alt_coef);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__timing);

  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter_update_scale__case1);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter_update_scale__case2);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter_update_scale__case3);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter_update_scale__case4);
  RUN_TEST_CASE(pixel_hfilter, pixel_hfilter_update_scale__timing);

  // RUN_TEST_CASE(pixel_hfilter, pixel_hfilter__case1);
}

TEST_GROUP(pixel_hfilter);
TEST_SETUP(pixel_hfilter) { fflush(stdout); }
TEST_TEAR_DOWN(pixel_hfilter) {}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter__basic)
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
TEST(pixel_hfilter, pixel_hfilter__acc_init)
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
TEST(pixel_hfilter, pixel_hfilter__apply_shift)
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
TEST(pixel_hfilter, pixel_hfilter__input_stride)
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
TEST(pixel_hfilter, pixel_hfilter__out_count)
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
TEST(pixel_hfilter, pixel_hfilter__alt_coef)
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
TEST(pixel_hfilter, pixel_hfilter__timing)
{
  static const unsigned max_blocks = 8;

  int8_t coef[32] = {0};
  int8_t input[32] = {0};
  int8_t output[16*max_blocks] = {0};

  unsigned timing[max_blocks];

  static const char func_name[] = "pixel_hfilter()";
  static const char row1_head[] = "out_count:";
  static const char row2_head[] = "ticks:    ";

  for(int k = 0; k < max_blocks; k++){
    unsigned ts = measure_time();
    pixel_hfilter(output, input, coef, 0, 0 , 0, 16*(k+1));
    unsigned te = measure_time();
    timing[k] = te - ts;
  }

  printf("\n\t%s timing:\n", func_name);
  printf("\t\t%s", row1_head);
  for(int k = 0; k < max_blocks; k++)   printf("%8u", 16*(k+1));
  printf("\n\t\t%s", row2_head);
  for(int k = 0; k < max_blocks; k++)   printf("%8u", timing[k]);
  printf("\n\n");

}






///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter_update_scale__case1)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 0.0f;
  const size_t offset = 0;

  pixel_hfilter_update_scale(&state, gain, offset);

  const unsigned exp_shift = 9;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = - 128 * shift_scale - SENSOR_BLACK_LEVEL * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0, state.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0, state.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0, state.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter_update_scale__case2)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 1.0f;
  const size_t offset = 1;

  pixel_hfilter_update_scale(&state, gain, offset);

  const unsigned exp_shift = 7;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = - SENSOR_BLACK_LEVEL * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x1B, state.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x4B, state.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x1B, state.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter_update_scale__case3)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 1.2f;
  const size_t offset = 0;

  pixel_hfilter_update_scale(&state, gain, offset);

  const unsigned exp_shift = 7;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * shift_scale - SENSOR_BLACK_LEVEL * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x20, state.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x59, state.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x20, state.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter_update_scale__case4)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 0.8f;
  const size_t offset = 1;

  pixel_hfilter_update_scale(&state, gain, offset);

  const unsigned exp_shift = 8;
  const int32_t shift_scale = 1 << exp_shift;
  const int32_t exp_acc_init = 128 * (gain - 1.0f) * shift_scale - SENSOR_BLACK_LEVEL * shift_scale;

  TEST_ASSERT_EQUAL_INT8(0x2B, state.coef[0 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x77, state.coef[2 + offset]);
  TEST_ASSERT_EQUAL_INT8(0x2B, state.coef[4 + offset]);

  TEST_ASSERT_EQUAL_UINT(exp_shift, state.shift);
  TEST_ASSERT_EQUAL_INT32(exp_acc_init, state.acc_init);
}

///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_hfilter, pixel_hfilter_update_scale__timing)
{
  hfilter_state_t state;

  memset(&state, 0, sizeof(state));

  const float gain = 1.0f;

  unsigned ts = measure_time();
  pixel_hfilter_update_scale(&state, gain, 1);
  unsigned te = measure_time();

  static const char func_name[] = "pixel_hfilter_update_scale()";

  printf("\n\t%s timing: %u ticks\n\n", func_name, te - ts);
}
