// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <xcore/port.h>
#include "xcore_compat.h"

// ----------- Mipi, packet, and shim definitions -------------------------

// Sensor defines
#define MODE_VGA_640x480            0x01       
#define MODE_1280x960               0x02

// Mipi defines
#define MIPI_IMAGE_WIDTH_BYTES      640
#define MIPI_IMAGE_HEIGHT_PIXELS    480
#define SENSOR_WIDHT                MIPI_IMAGE_WIDTH_BYTES
#define SENSOR_HEIGHT               MIPI_IMAGE_HEIGHT_PIXELS

// Mipi Configuration (could be modified by user)
#define CONFIG_MODE                 MODE_VGA_640x480
#define CONFIG_MIPI_FORMAT          MIPI_DT_RAW8
#define CONFIG_FLIP                 FLIP_NONE
#define CONFIG_BINNING              1
#define CONFIG_CENTRALISE           1

// Mipi payload
#define MIPI_MAX_PKT_SIZE_BYTES     ((MIPI_IMAGE_WIDTH_BYTES) + 4)
#define MIPI_PKT_BUFFER_COUNT       (4)

// MIPI packet header functions
#define MIPI_DATA_TYPE_MASK           (0x0000003F)
#define MIPI_LONG_PACKET_MASK         (0x30)

#define MIPI_HAS_PACKET_ERROR(STATUS) ((STATUS) & (0xFE))
#define MIPI_IS_LONG_PACKET(HEADER)   ((HEADER) & (MIPI_LONG_PACKET_MASK))
#define MIPI_GET_DATA_TYPE(HEADER)    ((HEADER) & (MIPI_DATA_TYPE_MASK))
#define MIPI_GET_WORD_COUNT(HEADER)   (((HEADER) >> 8) & 0xFFFF )

// Mipi shim configuration (MIPI_SHIM_CFG0)
#define MIPI_SHIM_CFG0_PACK(DMUX_EN, DMUX_DT, DMUX_MODE, DMUX_STFF, DMUX_BIAS) \
    ( (((DMUX_EN != 0)   & 0x1  ) << 0)     \
    | (((DMUX_DT)   & 0xFF ) << 8)          \
    | (((DMUX_MODE) & 0x3F ) << 16)         \
    | (((DMUX_STFF != 0) & 0x1  ) << 22)    \
    | (((DMUX_BIAS != 0) & 0x1  ) << 23))

// Mipi shim configuration (MIPI_SHIM_CFG3)
#define _ENABLE_LAN1       (1 << 14)
#define _ENABLE_LAN0       (1 << 13)
#define _ENABLE_CLK        (1 << 12)
#define _DPDN_SWAP_LAN1    (1 << 11)
#define _DPDN_SWAP_LAN0    (1 << 10)
#define _DPDN_SWAP_CLK     (1 << 9)

#define _LANE_SWAP_LAN1_DEFAULT (1 << 6)
#define _LANE_SWAP_LAN0_DEFAULT (0 << 3)
#define _LANE_SWAP_CLK_DEFAULT  (2 << 0)

#define DEFAULT_MIPI_DPHY_CFG3 ( \
  _ENABLE_LAN1  |\
  _ENABLE_LAN0  |\
  _ENABLE_CLK   |\
  _DPDN_SWAP_LAN1 |\
  _DPDN_SWAP_LAN0 |\
  _DPDN_SWAP_CLK |\
  _LANE_SWAP_LAN1_DEFAULT |\
  _LANE_SWAP_LAN0_DEFAULT |\
  _LANE_SWAP_CLK_DEFAULT \
)

// MIPI Shim configuration (MIPI_SHIM_CFG0) 
#define MIPI_SHIM_BIAS_ENABLE       1       //  Offset output pixels [1]
#define MIPI_SHIM_STUFF_ENABLE      0       // Add a zero byte after every RGB pixel [2]
#define MIPI_SHIM_DEMUX_MODE        0       // demux mode (see xMIPI_DemuxMode_t), Unused if MIPI_SHIM_DEMUX_EN = 0 
#define MIPI_SHIM_DEMUX_DATATYPE    0       // CSI-2 packet type to demux, Unused if MIPI_SHIM_DEMUX_EN = 0
#define MIPI_SHIM_DEMUX_EN          0       // MIPI shim 0 = disabled, 1 = enabled

// Mipi shim clock settings
#define MIPI_CLK_DIV      1     // CLK DIVIDER
#define MIPI_CFG_CLK_DIV  3     // CFG DIVIDER
#ifndef MIPI_CLKBLK
#define MIPI_CLKBLK XS1_CLKBLK_1
#endif



// ----------- Mipi, packet, and shim structures -------------------------

typedef unsigned mipi_header_t;

typedef enum {
  // 0x00 to 0x07 - Synchronization Short Packet Data Types
  MIPI_DT_FRAME_START = 0x00,
  MIPI_DT_FRAME_END = 0x01,
  MIPI_DT_LINE_START = 0x02,
  MIPI_DT_LINE_END = 0x03,
  MIPI_DT_EOT = 0x04,
  // MIPI_DT_RESERVED_0x05       = 0x05,
  // MIPI_DT_RESERVED_0x06       = 0x06,
  // MIPI_DT_RESERVED_0x07       = 0x07,

  // 0x08 to 0x0F - Generic Short Packet Data Types
  MIPI_DT_GENERIC_SHORT1 = 0x08,
  MIPI_DT_GENERIC_SHORT2 = 0x09,
  MIPI_DT_GENERIC_SHORT3 = 0x0A,
  MIPI_DT_GENERIC_SHORT4 = 0x0B,
  MIPI_DT_GENERIC_SHORT5 = 0x0C,
  MIPI_DT_GENERIC_SHORT6 = 0x0D,
  MIPI_DT_GENERIC_SHORT7 = 0x0E,
  MIPI_DT_GENERIC_SHORT8 = 0x0F,

  // 0x10 to 0x17 - Generic Long Packet Data Types
  MIPI_DT_NULL = 0x10,
  MIPI_DT_BLANKING_DATA = 0x11,
  MIPI_DT_8BIT_NONIMAGE = 0x12,
  MIPI_DT_GENERIC_LONG1 = 0x13,
  MIPI_DT_GENERIC_LONG2 = 0x14,
  MIPI_DT_GENERIC_LONG3 = 0x15,
  MIPI_DT_GENERIC_LONG4 = 0x16,
  MIPI_DT_RESERVED_0x17 = 0x17,

  // 0x18 to 0x1F - YUV Data
  MIPI_DT_YUV420_8BIT = 0x18,
  MIPI_DT_YUV420_10BIT = 0x19,
  MIPI_DT_YUV420_8BIT_LEGACY = 0x1A,
  MIPI_DT_YUV420_8BIT_CHROMA_SHIFTED = 0x1C,
  MIPI_DT_YUV420_10BIT_CHROMA_SHIFTED = 0x1D,
  MIPI_DT_YUV422_8BIT = 0x1E,
  MIPI_DT_YUV422_10BIT = 0x1F,

  // 0x20 to 0x26 - RGB Data
  MIPI_DT_RGB444 = 0x20,
  MIPI_DT_RGB555 = 0x21,
  MIPI_DT_RGB565 = 0x22,
  MIPI_DT_RGB666 = 0x23,
  MIPI_DT_RGB888 = 0x24,
  // MIPI_DT_RESERVED_0x25       = 0x25,
  // MIPI_DT_RESERVED_0x26       = 0x26,

  // 0x27 to 0x2F - RAW Data
  MIPI_DT_RAW24 = 0x27,
  MIPI_DT_RAW6 = 0x28,
  MIPI_DT_RAW7 = 0x29,
  MIPI_DT_RAW8 = 0x2A,
  MIPI_DT_RAW10 = 0x2B,
  MIPI_DT_RAW12 = 0x2C,
  MIPI_DT_RAW14 = 0x2D,
  MIPI_DT_RAW16 = 0x2E,
  MIPI_DT_RAW20 = 0x2F,

  // 0x30 to 0x37 - User Defined Byte-based Data
  MIPI_DT_USER_DEFINED1 = 0x30,
  MIPI_DT_USER_DEFINED2 = 0x31,
  MIPI_DT_USER_DEFINED3 = 0x32,
  MIPI_DT_USER_DEFINED4 = 0x33,
  MIPI_DT_USER_DEFINED5 = 0x34,
  MIPI_DT_USER_DEFINED6 = 0x35,
  MIPI_DT_USER_DEFINED7 = 0x36,
  MIPI_DT_USER_DEFINED8 = 0x37,

  // 0x38 - USL Commands
  MIPI_DT_USL = 0x38,

  // 0x39 to 0x3E - Reserved for future use
  // MIPI_DT_RESERVED_0x39       = 0x39,
  // MIPI_DT_RESERVED_0x3A       = 0x3A,
  // MIPI_DT_RESERVED_0x3B       = 0x3B,
  // MIPI_DT_RESERVED_0x3C       = 0x3C,
  // MIPI_DT_RESERVED_0x3D       = 0x3D,
  // MIPI_DT_RESERVED_0x3E       = 0x3E,

  // 0x3F - Unavailable (0x3F is used for LRTE EPD Spacer)

} mipi_data_type_t;

// alias
typedef mipi_data_type_t pixel_format_t;

typedef enum xMIPI_DemuxMode_t {
  XMIPI_DEMUXMODE_RESERVED = 0,
  XMIPI_DEMUXMODE_10TO16 = 1,
  XMIPI_DEMUXMODE_12TO16 = 2,
  XMIPI_DEMUXMODE_14TO16 = 3,
  XMIPI_DEMUXMODE_565TO88 = 4,
  XMIPI_DEMUXMODE_8TO10 = 5,
  XMIPI_DEMUXMODE_12TO8 = 6, // These are not in the shim documentation
  XMIPI_DEMUXMODE_14TO8 = 7  // These are not in the shim documentation
} xMIPI_DemuxMode_t;


typedef struct
{
    port_t p_mipi_clk;
    port_t p_mipi_rxa;
    port_t p_mipi_rxv;
    in_buffered_port_32_t p_mipi_rxd;
    xclock_t clk_mipi;
} camera_mipi_ctx_t;

// Represents a received MIPI packet.
typedef struct
{
  mipi_header_t header;
  uint8_t payload[MIPI_MAX_PKT_SIZE_BYTES];
} mipi_packet_t;

// Represents the state of the MIPI receiver.
typedef struct {
  unsigned wait_for_frame_start;
  unsigned frame_number;
  unsigned in_line_number;
  unsigned out_line_number;
} frame_state_t;

typedef enum {
  RES_640_480 = MODE_VGA_640x480,
  RES_1280_960 = MODE_1280x960
} resolution_t;

typedef enum{
  FLIP_NONE = (0 | (0 << 1)),
  FLIP_VERTICAL = (0 | (1 << 1))
} orientation_t;

/*
  Notes
  -----
  Notes from MIPI D-PHY specification and Mipi Shim documentation

  MIPI Data Typesfrom MIPI D-PHY specification
  --------------------------------------------
  0x00 to 0x07 - Synchronization Short Packet Data Types
  0x08 to 0x0F - Generic Short Packet Data Types
  0x10 to 0x17 - Generic Long Packet Data Types
  0x18 to 0x1F - YUV Data
  0x20 to 0x26 - RGB Data
  0x27 to 0x2F - RAW Data
  0x30 to 0x37 - User Defined Byte-based Data
  0x38 - USL Commands
  0x39 to 0x3E - Reserved for future use
  0x3F - Unavailable (0x3F is used for LRTE EPD Spacer)


  MIPI DPHY configuration register layout (MIPI_DPHY_CFG3)
  --------------------------------------------------------
    Bits    Name                    Meaning
    14      _ENABLE_LAN1            Set to 0 to disable lane 1 receiver
    13      _ENABLE_LAN0            Set to 0 to disable lane 0 receiver
    12      _ENABLE_CLK             Set to 0 to disable clock receiver
    11      _DPDN_SWAP_LAN1         Set to 1 to swap lane 1 polarity
    10      _DPDN_SWAP_LAN0         Set to 1 to swap lane 0 polarity
     9      _DPDN_SWAP_CLK          Set to 1 to swap clock polarity
    8:6     _LANE_SWAP_LAN1         The pin over which to input lane 1
    5:3     _LANE_SWAP_LAN0         The pin over which to input lane 0
    2:0     _LANE_SWAP_CLK          The pin over which to input clock


  MIPI Shim configuration register layout (MIPI_SHIM_CFG0)
  --------------------------------------------------------
    Bits    Name                    Meaning
    23      _BIAS                   Bias output pixels for VPU usage
    22      _DEMUX_STUFF            Add zero byte after every RGB pixel
    21:16   _PIXEL_DEMUX_MODE       Set demuxing mode
    15:8    _PIXEL_DEMUX_DATATYPE   CSI-2 packet type to demux
    0       _PIXEL_DEMUX_EN         Enable pixel demuxing
*/
