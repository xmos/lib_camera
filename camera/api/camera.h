#pragma once

#include <stdint.h>

#include "xs1.h"

#include "i2c.h"
#include "mipi.h"

#include "sensor.h"


#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

#define MIPI_TILE                 1

#include "image_hfilter.h"
#include "image_vfilter.h"
#include "packet_handler.h"
#include "statistics.h"
#include "user_api.h"
#include "utils.h"
#include "isp.h"


/**
 * The packet buffer is where the packet decoupler will tell the MIPI receiver
 * thread to store received packets.
 */
#define DEMUX_DATATYPE    0     // RESERVED
#define DEMUX_MODE        CONFIG_DEMUX_MODE // xor bias or not
#define DEMUX_EN          0     // DISABLE DEMUX
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER

#ifdef __XC__

/**
 * Thread entry point for interfacing with the camera sensor.
 * 
 * This version of the camera thread will perform decimation and demosaicing.
 * 
 * @param mipi_tile   The tile on which the MIPI receiver is located
 * @param p_mipi_clk  The MIPI clock input port
 * @param p_mipi_rxa  The MIPI active input port
 * @param p_mipi_rxv  The MIPI valid input port
 * @param p_mipi_rxd  The MIPI data input port
 * @param clk_mipi    The MIPI clock block
 * @param i2c         The I2C client interface
 * @param c_user_api  The user API channel
 */
void camera_main(
    tileref mipi_tile,
    in port p_mipi_clk,
    in port p_mipi_rxa,
    in port p_mipi_rxv,
    buffered in port:32 p_mipi_rxd,
    clock clk_mipi,
    client interface i2c_master_if i2c,
    streaming chanend c_user_api);

/**
 * Thread entry point for interfacing with the camera sensor.
 * 
 * This version of the camera thread will capture raw data.
 */
void camera_main_raw(
    tileref mipi_tile,
    in port p_mipi_clk,
    in port p_mipi_rxa,
    in port p_mipi_rxv,
    buffered in port:32 p_mipi_rxd,
    clock clk_mipi,
    client interface i2c_master_if i2c,
    streaming chanend c_user_api);

#endif //__XC__
