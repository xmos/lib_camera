#define MIPI_TILE                 1

// MIPI Shim configuration register layout (MIPI_SHIM_CFG0) 
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
