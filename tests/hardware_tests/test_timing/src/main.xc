// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "app.h"

// Camera control channels
void main_tile0(chanend_t c_control){
  sensor_control(c_control);
}

int main(void) 
{
  chan c_control;
  par {
    on tile[0]: main_tile0(c_control);
    on tile[MIPI_TILE]: mipi_main(c_control);
  }
  return 0;
}
