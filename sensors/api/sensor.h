

// -------------- Sensor abstraction layer. --------------
#define CONFIG_IMX219_SUPPORT 1
#define CONFIG_GC2145_SUPPORT 0

#if CONFIG_IMX219_SUPPORT
#include "imx219.h"
#endif

#if CONFIG_GC2145_SUPPORT
#include "gc2145.h"
#endif

// -------------- CAMERA MODELS
typedef enum
{
    CAMERA_IMX219,
    CAMERA_GC2145,
    CAMERA_NONE,
} camera_model_t;

// -------------- CAMERA MODES
typedef enum
{
    MODE_VGA_RAW10 = 0,
    MODE_QXVGA_YUV422
} frame_mode_t;

// -------------- CAMERA STRUCTURE
typedef struct
{
    const camera_model_t camera_model;
    const frame_mode_t frame_mode;
    // void (*init_func_ptr)(int);
    // void (*start_func_ptr)(int);

} camera_config_t;



// Pixel format definitions
/*
{
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_YUV422,    // 2BPP/YUV422
    PIXFORMAT_YUV420,    // 1.5BPP/YUV420
    PIXFORMAT_GRAYSCALE, // 1BPP/GRAYSCALE
    PIXFORMAT_JPEG,      // JPEG/COMPRESSED
    PIXFORMAT_RGB888,    // 3BPP/RGB888
    PIXFORMAT_RAW,       // RAW
    PIXFORMAT_RGB444,    // 3BP2P/RGB444
    PIXFORMAT_RGB555,    // 3BP2P/RGB555
}
*/

// framesize definitions
/*
    FRAMESIZE_96X96,   // 96x96
    FRAMESIZE_QQVGA,   // 160x120
    FRAMESIZE_QCIF,    // 176x144
    FRAMESIZE_HQVGA,   // 240x176
    FRAMESIZE_240X240, // 240x240
    FRAMESIZE_QVGA,    // 320x240
    FRAMESIZE_CIF,     // 400x296
    FRAMESIZE_HVGA,    // 480x320
    FRAMESIZE_VGA,     // 640x480
    FRAMESIZE_SVGA,    // 800x600
    FRAMESIZE_XGA,     // 1024x768
    FRAMESIZE_HD,      // 1280x720
    FRAMESIZE_SXGA,    // 1280x1024
    FRAMESIZE_UXGA,    // 1600x1200
    // 3MP Sensors
    FRAMESIZE_FHD,   // 1920x1080
    FRAMESIZE_P_HD,  //  720x1280
    FRAMESIZE_P_3MP, //  864x1536
    FRAMESIZE_QXGA,  // 2048x1536
    // 5MP Sensors
    FRAMESIZE_QHD,   // 2560x1440
    FRAMESIZE_WQXGA, // 2560x1600
    FRAMESIZE_P_FHD, // 1080x1920
    FRAMESIZE_QSXGA, // 2560x1920
    FRAMESIZE_INVALID
*/