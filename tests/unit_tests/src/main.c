// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <xscope.h>
#include "unity_fixture.h"

int main(
    int argc, 
    const char* argv[])
{
  xscope_config_io(XSCOPE_IO_BASIC);

  UnityGetCommandLineOptions(argc, argv);
  UnityBegin(argv[0]);

  printf("lib_camera Unit Tests\n");

  RUN_TEST_GROUP(color_conversion);
  RUN_TEST_GROUP(white_balance);
  RUN_TEST_GROUP(timings);
  //RUN_TEST_GROUP(pixel_hfilter);
  //RUN_TEST_GROUP(pixel_vfilter);
  //RUN_TEST_GROUP(gamma_timing);
  //RUN_TEST_GROUP(stats_test);
  //RUN_TEST_GROUP(resize_group);
  //RUN_TEST_GROUP(crop_group);
  
  
  return UNITY_END();
}
