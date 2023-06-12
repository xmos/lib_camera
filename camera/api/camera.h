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


/// Indicates whether the demux is enabled on the MIPI shim
/// 0 = disabled, 1 = enabled
#define MIPI_SHIM_DEMUX_EN          0
/// The MIPI datatype that is to be demuxed
/// Unused if MIPI_SHIM_DEMUX_EN = 0
#define MIPI_SHIM_DEMUX_DATATYPE    0
/// The MIPI shim demux mode (see xMIPI_DemuxMode_t)
/// Unused if MIPI_SHIM_DEMUX_EN = 0
#define MIPI_SHIM_DEMUX_MODE        0    
/// Indicates whether the MIPI shim should apply a bias to received pixel data.
/// 0 = disabled, 1 = enabled  (does not require MIPI_SHIM_DEMUX_EN = 1)
/// The bias subtracts 128 from each received pixel value, converting the
/// pixel data from uint8 to int8.
/// TODO: astew: This is currently ignored -- the bias is always enabled in the
///              downsample app, and disabled in the raw capture app.
#define MIPI_SHIM_BIAS_ENABLE       1
/// Indicates whether the MIPI shim should stuff an extra byte into each triplet
/// of received pixel data, in order to achieve 4-byte alignment of pixel
/// values.
/// 0 = disabled, 1 = enabled  (does not require MIPI_SHIM_DEMUX_EN = 1)
#define MIPI_SHIM_STUFF_ENABLE      0     // Enable byte stuffing


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
    client interface i2c_master_if i2c);

// /**
//  * Thread entry point for interfacing with the camera sensor.
//  * 
//  * This version of the camera thread will capture raw data.
//  */
// void camera_main_raw(
//     tileref mipi_tile,
//     in port p_mipi_clk,
//     in port p_mipi_rxa,
//     in port p_mipi_rxv,
//     buffered in port:32 p_mipi_rxd,
//     clock clk_mipi,
//     client interface i2c_master_if i2c,
//     streaming chanend c_user_api);

#endif //__XC__
