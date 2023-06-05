#pragma once
#include "xccompat.h"
#include "sensor.h"

#define CH APP_IMAGE_CHANNEL_COUNT
#define H APP_IMAGE_HEIGHT_PIXELS
#define W APP_IMAGE_WIDTH_PIXELS

// #define RAW_CAPTURE 0

// Image structure
typedef struct {
  int8_t pix[CH][H][W];
} image_t;

// User-side API
unsigned camera_capture_image(
    int8_t image_buff[CH][H][W],
    streaming_chanend_t c_cam_api);

// Framework-side API
void camera_api_init(streaming_chanend_t c_api);
void camera_api_request_begin();
void camera_api_request_complete();
void camera_api_request_update(
    const int8_t image_row[CH][W],
    const unsigned row_index);

// RAW
void camera_api_request_update_raw(uint16_t line_number, uint8_t *img_row_ptr);
void camera_api_request_complete_raw();
