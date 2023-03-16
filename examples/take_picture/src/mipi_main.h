#pragma once

#include <stdint.h>
#include "mipi.h"

#include "sensor.h"
///TODO this is inside lib_mipi
#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

// Must be large enough to fit a maximum-size long packet plus the 3-byte footer


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


#ifdef __XC__

void mipi_main(client interface i2c_master_if i2c);

#endif
