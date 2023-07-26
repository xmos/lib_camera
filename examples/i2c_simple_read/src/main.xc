#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>
#include <assert.h>

#include <xs1.h>
#include <platform.h> // for ports
#include <xccompat.h>
// #include <xcore/port.h> // NOT in XC :( 

#include "defines.h"
#include "mipi.h"

// #include "cam_i2c.h"

/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
on tile[MIPI_TILE] : in port p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE] : in port p_mipi_rxa = XS1_PORT_1E;               // activate
on tile[MIPI_TILE] : in port p_mipi_rxv = XS1_PORT_1I;               // valid
on tile[MIPI_TILE] : buffered in port:32 p_mipi_rxd = XS1_PORT_8A;   // data
on tile[MIPI_TILE] : clock clk_mipi = MIPI_CLKBLK;

void burn(){
  int i;
  for(i=0;i<1000000;i++);
}


void camera_main() 
{
  streaming chan c_stat_thread;
  streaming chan c_pkt;
  streaming chan c_ctrl;

  camera_mipi_init(
    p_mipi_clk,
    p_mipi_rxa,
    p_mipi_rxv,
    p_mipi_rxd,
    clk_mipi);
}

//extern void camera_i2c_master();

int main(void)
{ 
  par{
    on tile[MIPI_TILE]: camera_main();
    //on tile[0]: camera_i2c_master();
  }
  return 0;
}
