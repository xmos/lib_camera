#pragma once

// -------------- Sensor abstraction layer. --------------
#define CONFIG_IMX219_SUPPORT 1
#define CONFIG_GC2145_SUPPORT 0
#define CONFIG_MODE 0

#if CONFIG_IMX219_SUPPORT
#include "imx219.h"
#endif

#if CONFIG_GC2145_SUPPORT
#include "gc2145.h"
#endif

// Modes definition
#if CONFIG_MODE == 0
    #define FRAMESIZE_QVGA                  // 640x480
    #define EXPECTED_FORMAT MIPI_DT_RAW10   // RAW 10 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         800 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        480
    #define PIXEL_MULTIPLIER                1 // because is RAW 10, 1 byte per pyxel
#elif CONFIG_MODE == 1
    #define FRAMESIZE_UXGA                  // 1600x1200
    #define MIPI_IMAGE_WIDTH_PIXELS     1600
    #define MIPI_IMAGE_HEIGHT_PIXELS    1200
    #define PIXEL_MULTIPLIER 2
#endif

// Camera functions to be called from main program
#define camera_init(x)                  imx219_init(x)
#define camera_start(x)                 imx219_stream_start(x)
#define camera_configure(x)             imx219_configure_mode_0(x)


// -------------------------------------------------------------------------------
// Camera dependant (do not edit)
#define MIPI_LINE_WIDTH_BYTES ((MIPI_IMAGE_WIDTH_PIXELS)*PIXEL_MULTIPLIER) // because RAW 10
#define MIPI_PKT_BUFFER_COUNT 4 // this is user defined
#define MIPI_MAX_PKT_SIZE_BYTES ((MIPI_LINE_WIDTH_BYTES) + 4)
#define MIPI_TILE 1

// SRAM Image storage (do not edit)
extern uint8_t FINAL_IMAGE[MIPI_IMAGE_HEIGHT_PIXELS][MIPI_LINE_WIDTH_BYTES];
