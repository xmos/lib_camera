/*
TODO add compatibility with XC

*/
#include <stdio.h>
#include <platform.h>
#include <xcore/channel.h>
#include <xcore/port.h>
#include <xcore/parallel.h>
#include <xcore/hwtimer.h>
#include <xcore/select.h>
#include <xscope.h>
#include <platform.h>

#include "i2c.h"
#include "mipi.h"
#include "mipi_main.h"
#include "main.hpp"

// I2C interface ports
port_t p_scl = XS1_PORT_1N;
port_t p_sda = XS1_PORT_1O;

void enable_mipi_ports(){
    port_enable(p_scl);
    port_enable(p_sda);
}

// ?
// astew: TIL xscope_user_init() is an XC magic function that gets called
//        automatically..for some reason.
void xscope_user_init() {
   xscope_register(0, 0, "", 0, "");
   xscope_config_io(XSCOPE_IO_BASIC);
}


int main(void)
{
    enable_mipi_ports();

    // i2c par job on tile 0 
    i2c_master_init(
            i2c_ctx_ptr,
            p_scl, p_scl_bit_pos, 0,
            p_sda, p_sda_bit_pos, 0,
            SPEED);

    return 0;
}