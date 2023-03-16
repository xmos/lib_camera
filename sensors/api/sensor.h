

// -------------- Sensor abstraction layer. --------------
#define CONFIG_IMX219_SUPPORT 1
#define CONFIG_GC2145_SUPPORT 0

#if CONFIG_IMX219_SUPPORT
#include "imx219.h"
#define CAMERA_IMX219
#define FRAMESIZE_QVGA              // 640x480
#define PIXFORMAT_RAW10             // RAW 10 bits format
#define MIPI_IMAGE_WIDTH_PIXELS     800 // csi2 packed   
#define MIPI_IMAGE_HEIGHT_PIXELS    480
#define PIXEL_MULTIPLIER 1          // because is RAW 10, 1 byte per pyxel
#endif

#if CONFIG_GC2145_SUPPORT
#include "gc2145.h"
#define FRAMESIZE_UXGA        // 1600x1200
#define MIPI_IMAGE_WIDTH_PIXELS       1600
#define MIPI_IMAGE_HEIGHT_PIXELS      1200
#define PIXEL_MULTIPLIER 2
#endif


// Camera dependant
#define MIPI_LINE_WIDTH_BYTES ((MIPI_IMAGE_WIDTH_PIXELS)*PIXEL_MULTIPLIER) // because RAW 10
#define MIPI_PKT_BUFFER_COUNT 4 // this is user defined
#define MIPI_MAX_PKT_SIZE_BYTES ((MIPI_LINE_WIDTH_BYTES) + 4)
#define MIPI_TILE 1

// -------------- CAMERA MODELS
/*
#define CAMERA_GC2145 0x02
#define CAMERA_NONE   0x03  

// -------------- Pixel format definitions
#define PIXFORMAT_RGB565 0x00
#define PIXFORMAT_YUV422 0x01
#define PIXFORMAT_YUV420 0x02
#define PIXFORMAT_RGB888 0x03
#define PIXFORMAT_RAW10  0x04

// -------------- Framesize definitions
#define FRAMESIZE_QQVGA     0x01  // 160x120
#define FRAMESIZE_HQVGA     0x02 // 240x176
#define FRAMESIZE_240X240   0x03  // 240x240
#define FRAMESIZE_QVGA      0x04  // 320x240
#define FRAMESIZE_HVGA      0x05  // 480x320
#define FRAMESIZE_VGA       0x06  // 640x480
#define FRAMESIZE_HD        0x07  // 1280x720
#define FRAMESIZE_SXGA      0x08  // 1280x1024
#define FRAMESIZE_UXGA      0x09  // 1600x1200
#define FRAMESIZE_FHD       0x0A  // 1920x1080
#define FRAMESIZE_P_HD      0x0B  //  720x1280
#define FRAMESIZE_QHD       0x0C  // 2560x1440
#define FRAMESIZE_WQXGA     0x0D  // 2560x1600

*/

