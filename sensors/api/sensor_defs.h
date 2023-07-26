#pragma once

// ----- minimal commom definitions
#define DISABLED 0
#define ENABLED 1

#define MODE_VGA_640x480         0x01       
#define MODE_UXGA_1640x1232      0x02      
#define MODE_WQSXGA_3280x2464    0x03
#define MODE_FHD_1920x1080       0x04

#define _MIPI_DT_RAW8            0x2A
#define _MIPI_DT_RAW10           0x2B

#define BIAS_DISABLED            0x00  // no demux
#define BIAS_ENABLED             0x80  // bias
