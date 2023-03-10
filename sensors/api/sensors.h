

// -------------- Sensor abstraction layer. --------------

typedef enum
{
    CAMERA_IMX219,
    CAMERA_GC2145,
    CAMERA_NONE,
} camera_model_t;

typedef enum
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
} pixformat_t;

typedef enum
{
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
} framesize_t;

typedef struct
{
    const camera_model_t camera_model;
    const pixformat_t pix_format;
    const framesize_t framse_size;
} camera_sensor;
