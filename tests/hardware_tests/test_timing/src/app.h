
#pragma once

#include <stdint.h>

#include "mipi.h"
#include "camera.h"

#ifndef MIPI_CLKBLK
# define MIPI_CLKBLK                  XS1_CLKBLK_1
#endif


#ifdef __XC__

void mipi_main(client interface i2c_master_if i2c);

#endif
