
#pragma once

#include <stdint.h>

#include "mipi.h"

#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif

#define MIPI_IMAGE_WIDTH_PIXELS 800 //TODO this is camera dependant -> out of lib mipi
#define MIPI_IMAGE_HEIGHT_PIXELS 480 //TODO this is camera dependant -> out of lib mipi
 
#define MIPI_LINE_WIDTH_BYTES ((MIPI_IMAGE_WIDTH_PIXELS)*1)

#define MIPI_PKT_BUFFER_COUNT 4

// Must be large enough to fit a maximum-size long packet plus the 3-byte footer
#define MIPI_MAX_PKT_SIZE_BYTES ((MIPI_LINE_WIDTH_BYTES) + 4)
#define MIPI_TILE 1

typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;


#ifdef __XC__

void mipi_main(client interface i2c_master_if i2c);

#endif
