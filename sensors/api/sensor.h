// Sensor.h settings needed for custom sensor configuration
#pragma once

#define XSTR(x) STR(x)
#define STR(x) #x

// ----- minimal commom definitions
#define DISABLED 0
#define ENABLED 1

#define MODE_VGA_640x480         0x01       
#define MODE_UXGA_1640x1232      0x02      
#define MODE_WQSXGA_3280x2464    0x03
#define MODE_FHD_1920x1080       0x04
#define MODE_1280x960            0x05

#define _MIPI_DT_RAW8            0x2A
#define _MIPI_DT_RAW10           0x2B

#define BIAS_DISABLED            0x00  // no demux
#define BIAS_ENABLED             0x80  // bias

// -------------- Sensor abstraction layer. --------------
// Camera support
#define CONFIG_IMX219_SUPPORT   ENABLED
#define CONFIG_GC2145_SUPPORT   DISABLED

// Crop selection
#define CROP_ENABLED            DISABLED
#define CONFIG_MODE             MODE_1280x960

// Mipi format and mode
#ifndef CONFIG_MIPI_FORMAT
#define CONFIG_MIPI_FORMAT      _MIPI_DT_RAW8
#endif
#define MIPI_PKT_BUFFER_COUNT   4

// Black level settings
#define SENSOR_BLACK_LEVEL              16
// --------------------------------------------------------

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

#elif (CONFIG_MODE == MODE_1280x960)
    #define MIPI_IMAGE_WIDTH_PIXELS         1280
    #define MIPI_IMAGE_HEIGHT_PIXELS        960

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

// ----------------------------------------------------------------
// ----------------------------------------------------------------

#define SENSOR_BIT_DEPTH        (8)
#define SENSOR_RAW_IMAGE_WIDTH_PIXELS MIPI_IMAGE_WIDTH_PIXELS
#define SENSOR_RAW_IMAGE_HEIGHT_PIXELS MIPI_IMAGE_HEIGHT_PIXELS

#if     (CONFIG_MODE == MODE_VGA_640x480)
# define APP_DECIMATION_FACTOR    (4)
#elif   (CONFIG_MODE == MODE_1280x960)
# define APP_DECIMATION_FACTOR    (8)
#else
# error "Given CONFIG_MODE is not currently suported"
#endif

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


// Sensor control definitions

#define N_COMMANDS 5
typedef enum {
    SENSOR_INIT = 0,
    SENSOR_CONFIG,
    SENSOR_STREAM_START,
    SENSOR_STREAM_STOP,
    SENSOR_SET_EXPOSURE
} camera_control_t;
