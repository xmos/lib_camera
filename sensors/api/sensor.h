// Sensor.h settings needed for custom sensor configuration

#pragma once

#define XSTR(x) STR(x)
#define STR(x) #x

// ----- minimal comom definitions
#define ON                   1
#define OFF                  0

#define MODE_VGA_RAW10       0      // 640x480
#define MODE_VGA_RAW8        1      // 640x480
#define MODE_UXGA_RAW8       2      // 1640x1232
#define MODE_WQSXGA_RAW8     3      // 3280x2464
#define MODE_1920_1080       4

#define MIPI_DT_RAW8         0x2A
#define MIPI_DT_RAW10        0x2B

// -------------- Sensor abstraction layer. --------------
// This is user defined
#define CONFIG_IMX219_SUPPORT   ON
#define CONFIG_GC2145_SUPPORT   OFF
#define CONFIG_MODE             MODE_VGA_RAW8
#define MIPI_PKT_BUFFER_COUNT   4 
#define CROP_ENABLED            OFF

// FPS settings
// allowed values: [FPS_13, FPS_24, FPS_30, FPS_53, FPS_76]
#define FPS_13

// Inlcude custom libraries
#if CONFIG_IMX219_SUPPORT
    #include "imx219.h"
#endif

#if CONFIG_GC2145_SUPPORT
    #include "gc2145.h"
#endif


// Modes definition
#if (CONFIG_MODE == MODE_VGA_RAW10)
    #define EXPECTED_FORMAT MIPI_DT_RAW10   // RAW 10 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         640 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        480

#elif (CONFIG_MODE == MODE_VGA_RAW8)
    #define EXPECTED_FORMAT MIPI_DT_RAW8    // RAW 8 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         640
    #define MIPI_IMAGE_HEIGHT_PIXELS        480
   
#elif (CONFIG_MODE == MODE_UXGA_RAW8)
    #define EXPECTED_FORMAT MIPI_DT_RAW8   // RAW 8 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         1640 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        1232

#elif (CONFIG_MODE == MODE_WQSXGA_RAW8)
    #define EXPECTED_FORMAT MIPI_DT_RAW8   // RAW 8 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         3280 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        2464

#elif (CONFIG_MODE == MODE_1920_1080)
    #define EXPECTED_FORMAT MIPI_DT_RAW8   // RAW 8 bit data identifier
    #define MIPI_IMAGE_WIDTH_PIXELS         1920 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        1080

#endif


// Cropping mode
#if (CROP_ENABLED)
    #define CROP_WIDTH_PIXELS               320
    #define CROP_HEIGHT_PIXELS              240
    #define X_START_CROP                    0             
    #define Y_START_CROP                    0
    // crop checks
    #if (X_START_CROP + CROP_WIDTH_PIXELS) > MIPI_IMAGE_WIDTH_PIXELS
        #error Crop X dimensions must be inside bounds
    #endif
    // crop checks
    #if (Y_START_CROP + CROP_HEIGHT_PIXELS) > MIPI_IMAGE_HEIGHT_PIXELS
        #error Crop Y dimensions must be inside bounds
    #endif

#endif


// ----------------------- Settings dependant of each sensor library

// Camera functions to be called from main program
#define camera_init(x)                  imx219_init(x)
#define camera_start(x)                 imx219_stream_start(x)
#define camera_configure(x)             imx219_configure_mode(x)


// -------------------------------------------------------------------------------
#if EXPECTED_FORMAT == MIPI_DT_RAW10
    #define MIPI_IMAGE_WIDTH_BYTES (((MIPI_IMAGE_WIDTH_PIXELS) >> 2) * 5) // by 5/4

#elif EXPECTED_FORMAT == MIPI_DT_RAW8
    #define MIPI_IMAGE_WIDTH_BYTES MIPI_IMAGE_WIDTH_PIXELS // same size
#else
    #error Unknown format specified
#endif

// Camera dependant (do not edit)
#define MIPI_LINE_WIDTH_BYTES MIPI_IMAGE_WIDTH_BYTES
#define MIPI_MAX_PKT_SIZE_BYTES ((MIPI_LINE_WIDTH_BYTES) + 4)
#define MIPI_TILE 1

// SRAM Image storage (do not edit)
//TODO check maximum storage size for the image
#define MAX_MEMORY_SIZE 500000 << 2 //becasue half needed is code

#if MIPI_IMAGE_WIDTH_BYTES*MIPI_IMAGE_HEIGHT_PIXELS > MAX_MEMORY_SIZE
    #warning "The image appears to be too large for the available internal RAM.!"
#endif


