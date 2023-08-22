#pragma once

#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#include <xs1.h>
#include <platform.h>
#include <xscope.h>
#include <xccompat.h>

#include "camera_main.h"

/**
* Declaration of the MIPI interface ports:
* Clock, receiver active, receiver data valid, and receiver data
*/
extern in port p_mipi_clk;
extern in port p_mipi_rxa;
extern in port p_mipi_rxv;
extern buffered in port:32 p_mipi_rxd;
extern clock clk_mipi;

// this is the default process for the camera loop
void camera_process(chanend c_control);
