// Sensor.h settings needed for custom sensor configuration
#pragma once

#define XSTR(x) STR(x)
#define STR(x) #x

// -------------- Sensor abstraction layer. --------------
#include "sensor_defs.h"

// Camera support
#define CONFIG_IMX219_SUPPORT   ENABLED
#define CONFIG_GC2145_SUPPORT   DISABLED

// Crop selection
#define CROP_ENABLED            DISABLED
#define CONFIG_MODE             MODE_VGA_640x480

// Mipi format and mode
#ifndef CONFIG_MIPI_FORMAT
#define CONFIG_MIPI_FORMAT      _MIPI_DT_RAW8
#endif
#define MIPI_PKT_BUFFER_COUNT   4 

// FPS settings
#define FPS_13 // allowed values: [FPS_13, FPS_24, FPS_30, FPS_53, FPS_76]

// --------------------------------------------------------

// Include custom libraries
#if CONFIG_IMX219_SUPPORT
    #include "imx219.h"
    #define sensor_initialize(iic)          imx219_initialize(iic)
    #define sensor_stream_start(iic)        imx219_stream_start(iic)
    #define sensor_stream_stop(iic)         imx219_stream_stop(iic)
    #define sensor_configure(iic)           imx219_configure(iic)
    #define sensor_set_exposure(iic,ex)     imx219_set_exposure(iic,ex)
    #define SENSOR_BLACK_LEVEL              16
#endif

#if CONFIG_GC2145_SUPPORT
    #include "gc2145.h"
    /* //TODO
    #define sensor_initialize(iic)                gcinit(iic)
    #define sensor_stream_start(iic)               gcstart(iic)
    #define sensor_stream_stop(iic)                gcstop(iic)
    #define sensor_configure(iic)           gcconfigure(iic)
    #define sensor_set_exposure(iic,ex)     gcsetexp(iic,ex)
    #define SENSOR_BLACK_LEVEL              0
    */
#endif

// Modes configurations
#ifndef CONFIG_MODE
    #error CONFIG_MODE has to be defined
#endif

#if (CONFIG_MODE == MODE_VGA_640x480)
    #define MIPI_IMAGE_WIDTH_PIXELS         640 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        480

#elif (CONFIG_MODE == MODE_UXGA_1640x1232)
    #define MIPI_IMAGE_WIDTH_PIXELS         1640 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        1232

#elif (CONFIG_MODE == MODE_WQSXGA_3280x2464)
    #define MIPI_IMAGE_WIDTH_PIXELS         3280 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        2464

#elif (CONFIG_MODE == MODE_FHD_1920x1080)
    #define MIPI_IMAGE_WIDTH_PIXELS         1920 // csi2 packed (stride 800) 
    #define MIPI_IMAGE_HEIGHT_PIXELS        1080

#else 
    #error Unknown configuration mode
#endif

// Pixel format configurations
#ifndef CONFIG_MIPI_FORMAT
    #error CONFIG_MIPI_FORMAT has to be specified
#else
    #if (CONFIG_MIPI_FORMAT == _MIPI_DT_RAW10)
        #define MIPI_IMAGE_WIDTH_BYTES (((MIPI_IMAGE_WIDTH_PIXELS) >> 2) * 5) // by 5/4

    #elif (CONFIG_MIPI_FORMAT == _MIPI_DT_RAW8)
        #define MIPI_IMAGE_WIDTH_BYTES MIPI_IMAGE_WIDTH_PIXELS // same size

    #else
        #error CONFIG_MIPI_FORMAT not supported
    #endif
#endif

// Cropping configurations
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

// Camera dependant (do not edit)
#define MIPI_MAX_PKT_SIZE_BYTES ((MIPI_IMAGE_WIDTH_BYTES) + 4)
#define MIPI_TILE 1
#define EXPECTED_FORMAT CONFIG_MIPI_FORMAT //backward compatibility
#define MIPI_EXPECTED_FORMAT CONFIG_MIPI_FORMAT //backward compatibility
// SRAM Image storage (do not edit)
//TODO check maximum storage size for the image
#define MAX_MEMORY_SIZE 500000 << 2 //becasue half needed is code

//#if MIPI_IMAGE_WIDTH_BYTES*MIPI_IMAGE_HEIGHT_PIXELS > MAX_MEMORY_SIZE
//    #warning "The image appears to be too large for the available internal RAM.!"
//#endif

// ----------------------------------------------------------------
// ----------------------------------------------------------------

#define SENSOR_BIT_DEPTH        (8)
#define SENSOR_RAW_IMAGE_WIDTH_PIXELS MIPI_IMAGE_WIDTH_PIXELS
#define SENSOR_RAW_IMAGE_HEIGHT_PIXELS MIPI_IMAGE_HEIGHT_PIXELS

#define APP_DECIMATION_FACTOR    (4)

#define VPU_SIZE_16B             (16)

#define NOT_PADDED_WIDTH_PIXELS   (SENSOR_RAW_IMAGE_WIDTH_PIXELS \
                                   / APP_DECIMATION_FACTOR)

#define PADDED_WIDTH_PIXELS       (((NOT_PADDED_WIDTH_PIXELS / VPU_SIZE_16B) + 1) \
                                   * VPU_SIZE_16B)

#if ((NOT_PADDED_WIDTH_PIXELS % VPU_SIZE_16B) != 0)
# define APP_IMAGE_WIDTH_PIXELS   PADDED_WIDTH_PIXELS
#else
# define APP_IMAGE_WIDTH_PIXELS   NOT_PADDED_WIDTH_PIXELS
#endif

#define APP_IMAGE_HEIGHT_PIXELS   (SENSOR_RAW_IMAGE_HEIGHT_PIXELS \
                                   / APP_DECIMATION_FACTOR)

#define APP_IMAGE_CHANNEL_COUNT   (3)

#define CHAN_RED    0
#define CHAN_GREEN  1
#define CHAN_BLUE   2

// Number of bits to collapse channel cardinality (larger value results in fewer
// histogram bins)
#ifndef HIST_QUANT_BITS
#define HIST_QUANT_BITS   (2)
#endif

// Not every pixel of the image will be sampled. This is the distance between
// sampled values in a row.
#ifndef APP_HISTOGRAM_SAMPLE_STEP
#define APP_HISTOGRAM_SAMPLE_STEP   (1)
#endif

// For simplicity here
#define CH  (APP_IMAGE_CHANNEL_COUNT)
#define H   (APP_IMAGE_HEIGHT_PIXELS)
#define W   (APP_IMAGE_WIDTH_PIXELS)

#define H_RAW   (MIPI_IMAGE_HEIGHT_PIXELS)
#define W_RAW   (MIPI_IMAGE_WIDTH_BYTES)
