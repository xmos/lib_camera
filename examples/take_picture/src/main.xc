// Copyright (c) 2020, XMOS Ltd, All rights reserved
#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "i2c.h"
#include "mipi_main.h"

// I2C interface ports
#define Kbps 400
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;

int main(void) 
{
    i2c_master_if i2c[1];
    par {
        on tile[0]: i2c_master(i2c, 1, p_scl, p_sda, Kbps);
        on tile[MIPI_TILE]: mipi_main(i2c[0]);
        // on tile[MIPI_TILE]: mipi_main(i2c[0], &camera_config);
    }
    return 0;
}
