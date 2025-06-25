// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>

extern "C" {
#include "xscope_io_device.h"
}

extern "C" {
  void app();
}

int main(void)
{
  // xscope
  chan xscope_chan;
  
  // Parallel jobs
  par{
    xscope_host_data(xscope_chan);
    on tile[1]: xscope_io_init(xscope_chan);
    on tile[1]: app();
  }
  return 0;
}
