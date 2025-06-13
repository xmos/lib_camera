// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>

#include "camera.h"

extern "C" {
void user_app(chanend_t c_user_isp);
}

int main(void)
{
  chan c_cam;
  chan c_i2c;
  
  // Parallel jobs
  par{
    on tile[0]: camera_main_tile0(c_i2c);
    on tile[1]: camera_main_tile1(c_cam, c_i2c);
    on tile[1]: user_app(c_cam);
  }
  return 0;
}
