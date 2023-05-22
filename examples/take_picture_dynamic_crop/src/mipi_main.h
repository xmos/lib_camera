#pragma once

#include <stdint.h>
#include "mipi.h"

#include "sensor.h"

///TODO this is inside lib_mipi
#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

/**
 * The packet buffer is where the packet decoupler will tell the MIPI receiver
 * thread to store received packets.
 */
#define DEMUX_DATATYPE    0     // RESERVED
#define DEMUX_MODE        0x00  // no demux
#define DEMUX_EN          0     // DISABLE DEMUX
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER
#define REV(n) ((n << 24) | (((n>>16)<<24)>>16) |  (((n<<16)>>24)<<16) | (n>>24))

// Packets definitions
typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_LINE_WIDTH_BYTES];
} mipi_data_t;

typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;

typedef struct
{
  unsigned frame_number;
  unsigned line_number;
} image_rx_t;


static mipi_packet_t packet_buffer[MIPI_PKT_BUFFER_COUNT];

// Functions definitions
#define gMipiPacketRx(...)  MipiPacketRxnrev(__VA_ARGS__) // define your own function to use

#ifdef __XC__

void mipi_main(client interface i2c_master_if i2c);

#endif
