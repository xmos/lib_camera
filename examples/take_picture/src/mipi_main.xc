#include <xs1.h>
#include <print.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <platform.h>
#include <string.h>
#include <math.h>
#include "i2c.h"

// MIPI
#include "mipi.h"
#include "mipi_main.h"
#include "extmem_data.h"
#include "mipi/MipiPacket.h"

/*


*/

/*
#include "gc2145.h"
#include "debayer.h"
#include "yuv_to_rgb.h"
*/

/* Declaration of the MIPI interface ports:
 * Clock, receiver active, receiver data valid, and receiver data
 */
on tile[MIPI_TILE]:         in port    p_mipi_clk = XS1_PORT_1O;
on tile[MIPI_TILE]:         in port    p_mipi_rxa = XS1_PORT_1E;
on tile[MIPI_TILE]:         in port    p_mipi_rxv = XS1_PORT_1I;
on tile[MIPI_TILE]:buffered in port:32 p_mipi_rxd = XS1_PORT_8A;

on tile[MIPI_TILE]:clock               clk_mipi   = MIPI_CLKBLK;


void mipi_main(client interface i2c_master_if i2c)
{
    printf("hello world\n");
}