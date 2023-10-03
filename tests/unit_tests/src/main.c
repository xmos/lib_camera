// Copyright 2023 XMOS LIMITED.
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

  printf("\n");

  RUN_TEST_GROUP(pixel_hfilter);
  RUN_TEST_GROUP(pixel_vfilter);
  RUN_TEST_GROUP(color_conversion);
  RUN_TEST_GROUP(gamma_timing);
  RUN_TEST_GROUP(stats_test);
  
  return UNITY_END();
}
