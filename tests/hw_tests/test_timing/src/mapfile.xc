// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>

#include "camera.h"

extern "C" {
void user_app(chanend_t c_cam);
void camera_main_xscope(chanend_t c_cam);
}

int main(void)
{
  chan c_cam;
  
  // Parallel jobs
  par{
    on tile[1]: camera_main_xscope(c_cam);
    on tile[1]: user_app(c_cam);
  }
  return 0;
}
