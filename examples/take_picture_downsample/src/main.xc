// Copyright (c) 2020, XMOS Ltd, All rights reserved
#include <xs1.h>
#include <platform.h>
#include <xscope.h>

#include "i2c.h"
#include "camera.h"
#include "app.h"

// I2C interface ports
#define Kbps 400
on tile[0]: port p_scl = XS1_PORT_1N;
on tile[0]: port p_sda = XS1_PORT_1O;


/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A;   // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;


int main(void) 
{
  i2c_master_if i2c[1];
  par {
    on tile[0]: i2c_master(i2c, 1, p_scl, p_sda, Kbps);

    on tile[MIPI_TILE]: camera_main(tile[MIPI_TILE],
                                    p_mipi_clk, 
                                    p_mipi_rxa, 
                                    p_mipi_rxv, 
                                    p_mipi_rxd, 
                                    clk_mipi, 
                                    i2c[0]);
                                    
    on tile[MIPI_TILE]: user_app();
  }
  return 0;
}
