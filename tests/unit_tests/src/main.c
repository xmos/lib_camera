// Copyright 2020-2022 XMOS LIMITED.
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

  RUN_TEST_GROUP(image_hfilter);
  RUN_TEST_GROUP(isp_tests);
  
  return UNITY_END();
}