// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <xscope.h>
#include "unity_fixture.h"

int main(
    int argc, 
    const char* argv[])
{
  UnityGetCommandLineOptions(argc, argv);
  UnityBegin(argv[0]);

  printf("lib_camera Unit Tests\n");
 
  RUN_TEST_GROUP(color_conversion);
  RUN_TEST_GROUP(white_balance);
  RUN_TEST_GROUP(timings);
  RUN_TEST_GROUP(yuv);
  
  return UNITY_END();
}
