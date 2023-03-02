#TODO add compatibility with XC

#include <stdio.h>
#include <xs1.h>
#include <xcore/port.h>
#include "mipi.h"

/*
on tile[MIPI_TILE]:         in port    p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE]:         in port    p_mipi_rxa = XS1_PORT_1E;
on tile[MIPI_TILE]:         in port    p_mipi_rxv = XS1_PORT_1I;
on tile[MIPI_TILE]:buffered in port:32 p_mipi_rxd = XS1_PORT_8A;
*/

int main(void)
{
    port_t p_mipi_clk = XS1_PORT_1O; 
    port_enable(p_mipi_clk);

    unsigned long clk_state = port_in(p_mipi_clk); // i force to be equal at the beginning
    // power on the sensor
    // write_node_config_reg(tile, XS1_SSWITCH_MIPI_DPHY_CFG3_NUM , 0x7E42);

    // config camera 
    printf("clk_state = %lu\n", clk_state);
    // take frame

    return 0;
}