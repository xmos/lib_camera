// Copyright 2020-2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>

#include "unity_fixture.h"

#include "camera_main.h"

TEST_GROUP_RUNNER(pixel_vfilter) {
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_acc_init__case0);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_acc_init__case1);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_acc_init__timing);

  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_complete__case0);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_complete__case1);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_complete__timing);

  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_macc__case0);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_macc__case1);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_macc__case2);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_macc__case3);
  RUN_TEST_CASE(pixel_vfilter, pixel_vfilter_macc__timing);
}

#define ACC_HI(X)     (((X)>>16)&0xFFFF)
#define ACC_LO(X)     (((X)>> 0)&0xFFFF)



static 
int debug_iter = -1;


TEST_GROUP(pixel_vfilter);
TEST_SETUP(pixel_vfilter) { 
  fflush(stdout); 
  debug_iter = -1;
}

TEST_TEAR_DOWN(pixel_vfilter) {
  if(debug_iter != -1){
    printf("\nDEBUG:\n");
    printf("\tdebug_iter = %d\n", debug_iter);
  }
  debug_iter = -1;
}




///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_acc_init__case0)
{
  static const unsigned acc_blocks = 1;

  struct {
    uint32_t pre_padding;
    struct {
      int16_t hi[16];
      int16_t lo[16];
    } accs[acc_blocks];
    uint32_t post_padding;  
  } thing;

  // These ensure that the function doesn't write outside of the bounds of the
  // array
  thing.pre_padding = 0x52C6ABE2;
  thing.post_padding = 0xABCD1234;

  static const
  int32_t test_cases[] = {
    0x00000000, 0x00000001, 0x00010000, -0x00000001, 0x12345678,
  };

  static const
  unsigned n_cases = sizeof(test_cases) / sizeof(test_cases[0]);

  for(int iter = 0; iter < n_cases; iter++){
    debug_iter = iter; // For debug on failure

    const int32_t acc_value = test_cases[iter];

    memset(&thing.accs, 0, sizeof(thing.accs));

    pixel_vfilter_acc_init(&thing.accs[0].hi[0], acc_value, 16*acc_blocks);

    for(int k = 0; k < acc_blocks; k++){
      for(int i = 0; i < 16; i++){
        TEST_ASSERT_EQUAL_INT16(ACC_HI(acc_value), thing.accs[k].hi[i]);
        TEST_ASSERT_EQUAL_INT16(ACC_LO(acc_value), thing.accs[k].lo[i]);
      }
    }
    TEST_ASSERT_EQUAL_UINT32(0x52C6ABE2, thing.pre_padding);
    TEST_ASSERT_EQUAL_UINT32(0xABCD1234, thing.post_padding);
  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_acc_init__case1)
{
  static const unsigned acc_blocks = 4;

  struct {
    uint32_t pre_padding;
    struct {
      int16_t hi[16];
      int16_t lo[16];
    } accs[acc_blocks];
    uint32_t post_padding;  
  } thing;

  // These ensure that the function doesn't write outside of the bounds of the
  // array
  thing.pre_padding = 0x52C6ABE2;
  thing.post_padding = 0xABCD1234;

  if(1){
    const int32_t acc_value = 0x12345678;
    memset(&thing.accs, 0, sizeof(thing.accs));
    pixel_vfilter_acc_init(&thing.accs[0].hi[0], acc_value, 16*acc_blocks);
    for(int k = 0; k < acc_blocks; k++){
      for(int i = 0; i < 16; i++){
        TEST_ASSERT_EQUAL_INT16(ACC_HI(acc_value), thing.accs[k].hi[i]);
        TEST_ASSERT_EQUAL_INT16(ACC_LO(acc_value), thing.accs[k].lo[i]);
      }
    }
    TEST_ASSERT_EQUAL_UINT32(0x52C6ABE2, thing.pre_padding);
    TEST_ASSERT_EQUAL_UINT32(0xABCD1234, thing.post_padding);
  }
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_acc_init__timing)
{
  static const unsigned acc_blocks = 8;

  int32_t accs[16*acc_blocks];

  unsigned timing[acc_blocks];

  static const char func_name[] = "pixel_vfilter_acc_init()";
  static const char row1_head[] = "out_count:";
  static const char row2_head[] = "ticks:    ";

  for(int k = 0; k < acc_blocks; k++){
    unsigned ts = measure_time();
    pixel_vfilter_acc_init((int16_t*) &accs, 0, 16*(k+1));
    unsigned te = measure_time();
    timing[k] = te - ts;
  }

  printf("\n\t%s timing:\n", func_name);
  printf("\t\t%s", row1_head);
  for(int k = 0; k < acc_blocks; k++)   printf("%8u", 16*(k+1));
  printf("\n\t\t%s", row2_head);
  for(int k = 0; k < acc_blocks; k++)   printf("%8u", timing[k]);
  printf("\n\n");
}






///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_complete__case0)
{
  static const unsigned acc_per_vec = 16;
  static const unsigned acc_blocks = 1;
  static const unsigned output_pixels = acc_blocks*acc_per_vec;

  struct {
    int16_t hi[acc_per_vec];
    int16_t lo[acc_per_vec];
  } accs[acc_blocks];

  int8_t output[output_pixels];
  int16_t shifts[acc_per_vec];

  int8_t expected_out[output_pixels] = {0};

  struct {
    int32_t acc_value;
    int16_t shift;
    int8_t expected_out;
  } test_cases[] = {
    // accumulator    shift     output
    {   0x00000000,       0,      0x00},
    {   0x00000001,       0,      0x01},
    {  -0x00000001,       0,     -0x01},
    {   0x00000002,       1,      0x01},
    {   0x00000004,       1,      0x02},
    {   0x00000004,       2,      0x01},
    {  -0x00000004,       1,     -0x02},
    {   0x0000007F,       0,      0x7F},
    {  -0x0000007F,       0,     -0x7F},
    {  -0x00000080,       0,     -0x7F},
    {   0x00000100,       0,      0x7F},
    {   0x007E0000,      16,      0x7E},
  };

  static const unsigned n_cases = sizeof(test_cases)/sizeof(test_cases[0]);

  for(int iter = 0; iter < n_cases; iter++){
    debug_iter = iter; // For debug on failure

    const int32_t acc_value = test_cases[iter].acc_value;

    for(int k = 0; k < acc_per_vec; k++) 
      shifts[k] = test_cases[iter].shift;

    for(int k = 0; k < output_pixels; k++) 
      expected_out[k] = test_cases[iter].expected_out;

    pixel_vfilter_acc_init(&accs[0].hi[0], acc_value, output_pixels);

    pixel_vfilter_complete(output, &accs[0].hi[0], shifts, output_pixels);

    TEST_ASSERT_EQUAL_INT8_ARRAY(expected_out, output, output_pixels);
  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_complete__case1)
{
  static const unsigned acc_per_vec = 16;
  static const unsigned acc_blocks = 6;
  static const unsigned output_pixels = acc_blocks*acc_per_vec;

  struct {
    int16_t hi[acc_per_vec];
    int16_t lo[acc_per_vec];
  } accs[acc_blocks];

  pixel_vfilter_acc_init(&accs[0].hi[0], 0x007E0000, output_pixels);

  int16_t shifts[acc_per_vec];
  for(int k = 0; k < acc_per_vec; k++) shifts[k] = 16;
  
  // Add 16 bytes of padding around either side of the array to ensure that the
  // function doesn't write outside of the bounds.
  int8_t output[output_pixels + 32];
  int8_t expected_out[output_pixels + 32];

  const int8_t expected = 0x7E;

  for(int blocks = 1; blocks < acc_blocks; blocks++){
    debug_iter = blocks; // For debug on failure

    memset(output, 0, sizeof(output));
    memset(expected_out, 0, sizeof(expected_out));

    for(int k = 0; k < 16*blocks; k++)
      expected_out[k+16] = expected;

    pixel_vfilter_complete(&output[16], &accs[0].hi[0], shifts, 16*blocks);

    TEST_ASSERT_EQUAL_INT8_ARRAY(expected_out, output, output_pixels + 32);
  }

  debug_iter = -1; // signals not to print debug on teardown
}



///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_complete__timing)
{
  static const unsigned max_blocks = 8;

  int32_t accs[16*max_blocks];
  int8_t output[16*max_blocks];
  int16_t shifts[16] = {0};

  unsigned timing[max_blocks];

  static const char func_name[] = "pixel_vfilter_complete()";
  static const char row1_head[] = "out_count:";
  static const char row2_head[] = "ticks:    ";

  for(int k = 0; k < max_blocks; k++){
    unsigned ts = measure_time();
    pixel_vfilter_complete(output, (int16_t*) &accs, shifts, 16*(k+1));
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
TEST(pixel_vfilter, pixel_vfilter_macc__case0)
{
  static const unsigned acc_per_vec = 16;
  static const unsigned acc_blocks = 1;
  static const unsigned out_count = acc_per_vec*acc_blocks;

  int32_t accs[out_count];
  int8_t coef[16];
  int8_t pixels_in[out_count];

  int32_t expected_accs[out_count];

  struct {
    int32_t acc_init;
    int8_t coef;
    int8_t pixel;
    int32_t expected_acc;
  } test_case[] = {
    //      acc_init,   coef,   pixel,      expected,
    {     0x00000000,   0x00,    0x00,    0x00000000,   },
    {     0x00000001,   0x00,    0x00,    0x00000001,   },
    {     0x00001234,   0x00,    0x01,    0x00001234,   },
    {     0x00001234,   0x01,    0x00,    0x00001234,   },
    {     0x00001234,   0x01,    0x01,    0x00001235,   },
    {    -0x00001234,   0x01,    0x02,   -0x00001232,   },
    {     0x00010000,   0x0A,    0x10,    0x000100A0,   },
  };

  static const
  unsigned n_cases = sizeof(test_case)/sizeof(test_case[0]);

  for(int iter = 0; iter < n_cases; iter++){
    debug_iter = iter; // For debug on failure

    int32_t acc_init = test_case[iter].acc_init;
    int8_t coeff = test_case[iter].coef;
    int8_t pixel = test_case[iter].pixel;
    int32_t expected_acc = test_case[iter].expected_acc;

    pixel_vfilter_acc_init((int16_t*) &accs[0], acc_init, out_count);
    pixel_vfilter_acc_init((int16_t*) &expected_accs[0], expected_acc, out_count);

    memset(coef, coeff, sizeof(coef));
    memset(pixels_in, pixel, sizeof(pixels_in));

    pixel_vfilter_macc((int16_t*) &accs[0], pixels_in, coef, out_count);

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected_accs, accs, out_count);

  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_macc__case1)
{
  static const unsigned acc_per_vec = 16;
  static const unsigned max_blocks = 4;
  static const unsigned max_out_count = acc_per_vec*max_blocks;

  int32_t accs[max_out_count];
  int8_t coef[16];
  int8_t pixels_in[max_out_count];

  int32_t expected_accs[max_out_count];


  for(int blocks = 1; blocks <= max_blocks; blocks++){
    debug_iter = blocks-1; // For debug on failure

    unsigned out_count = acc_per_vec*blocks;

    int32_t acc_init = 0x00010000;
    int8_t coeff = 0x05;
    int8_t pixel = 0x20;
    int32_t expected_acc = 0x000100A0;

    memset(accs, 0, sizeof(accs));
    memset(expected_accs, 0, sizeof(expected_accs));
    pixel_vfilter_acc_init((int16_t*) &accs[0], acc_init, out_count);
    pixel_vfilter_acc_init((int16_t*) &expected_accs[0], expected_acc, out_count);

    memset(coef, coeff, sizeof(coef));
    memset(pixels_in, pixel, sizeof(pixels_in));

    pixel_vfilter_macc((int16_t*) &accs[0], pixels_in, coef, out_count);

    // Compare all the way to end to make sure it doesn't write past end
    TEST_ASSERT_EQUAL_INT32_ARRAY(expected_accs, accs, max_out_count);

  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_macc__case2)
{  
  static const unsigned acc_per_vec = 16;

  int16_t accs[2][acc_per_vec];
  int8_t coef[acc_per_vec];
  int8_t pixels_in[acc_per_vec];

  int32_t expected_accs[acc_per_vec];

  int32_t result_accs[acc_per_vec];

  struct {
    int32_t acc_init;
    int8_t coef;
    int8_t pixel;
  } test_case[] = {
    //      acc_init,   coef,   pixel  
    {     0x00000000,   0x00,    0x00},
    {     0x00000001,   0x00,    0x00},
    {     0x00001234,   0x00,    0x01},
    {     0x00001234,   0x01,    0x00},
    {     0x00001234,   0x01,    0x01},
    {    -0x00001234,   0x01,    0x02},
    {     0x00010000,   0x0A,    0x10},
  };

  static const
  unsigned n_cases = sizeof(test_case)/sizeof(test_case[0]);

  for(int iter = 0; iter < n_cases; iter++){
    debug_iter = iter; // For debug on failure

    int32_t acc_init = test_case[iter].acc_init;
    int8_t coeff = test_case[iter].coef;
    int8_t pixel = test_case[iter].pixel;

    pixel_vfilter_acc_init(&accs[0][0], acc_init, acc_per_vec);

    memset(coef, coeff, sizeof(coef));
    for(int k = 0; k < acc_per_vec; k++)
      pixels_in[k] = pixel + k;

    for(int k = 0; k < acc_per_vec; k++)
      expected_accs[k] = acc_init + coeff * (pixel + k);

    pixel_vfilter_macc((int16_t*) &accs[0], pixels_in, coef, acc_per_vec);

    for(int k = 0; k < acc_per_vec; k++){
      result_accs[k] = (accs[0][k] << 16) | (accs[1][k]);
    }

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected_accs, result_accs, acc_per_vec);

  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_macc__case3)
{  
  static const unsigned acc_per_vec = 16;

  int16_t accs[2][acc_per_vec];
  int8_t coef[acc_per_vec];
  int8_t pixels_in[acc_per_vec];

  int32_t expected_accs[acc_per_vec];

  int32_t result_accs[acc_per_vec];

  struct {
    int32_t acc_init;
    int8_t coef;
    int8_t pixel;
  } test_case[] = {
    //      acc_init,   coef,   pixel  
    {     0x00000000,   0x00,    0x00},
    {     0x00000001,   0x00,    0x00},
    {     0x00001234,   0x00,    0x01},
    {     0x00001234,   0x01,    0x00},
    {     0x00001234,   0x01,    0x01},
    {    -0x00001234,   0x01,    0x02},
    {     0x00010000,   0x0A,    0x10},
  };

  static const
  unsigned n_cases = sizeof(test_case)/sizeof(test_case[0]);

  for(int iter = 0; iter < n_cases; iter++){
    debug_iter = iter; // For debug on failure

    int32_t acc_init = test_case[iter].acc_init;
    int8_t coeff = test_case[iter].coef;
    int8_t pixel = test_case[iter].pixel;

    for(int k = 0; k < acc_per_vec; k++){
      accs[0][k] = ACC_HI(acc_init+7*k);
      accs[1][k] = ACC_LO(acc_init+7*k);
    }

    memset(coef, coeff, sizeof(coef));
    for(int k = 0; k < acc_per_vec; k++)
      pixels_in[k] = pixel + k;

    for(int k = 0; k < acc_per_vec; k++)
      expected_accs[k] = acc_init + 7*k + coeff * (pixel + k);

    pixel_vfilter_macc((int16_t*) &accs[0], pixels_in, coef, acc_per_vec);

    for(int k = 0; k < acc_per_vec; k++){
      result_accs[k] = (accs[0][k] << 16) | (accs[1][k]);
    }

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected_accs, result_accs, acc_per_vec);

  }

  debug_iter = -1; // signals not to print debug on teardown
}


///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
TEST(pixel_vfilter, pixel_vfilter_macc__timing)
{
  static const unsigned acc_per_vec = 16;
  static const unsigned max_blocks = 8;
  static const unsigned max_out_count = acc_per_vec*max_blocks;

  int32_t accs[max_out_count];
  int8_t coef[16];
  int8_t pixels_in[max_out_count];

  unsigned timing[max_blocks];

  static const char func_name[] = "pixel_vfilter_macc()";
  static const char row1_head[] = "out_count:";
  static const char row2_head[] = "ticks:    ";

  for(int k = 0; k < max_blocks; k++){
    unsigned blocks = k+1;
    unsigned out_count = acc_per_vec*blocks;

    unsigned ts = measure_time();
    pixel_vfilter_macc((int16_t*) &accs[0], pixels_in, coef, out_count);
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
