#pragma once
#include "xccompat.h"
#include "sensor.h"

#define CH  (APP_IMAGE_CHANNEL_COUNT)
#define H   (APP_IMAGE_HEIGHT_PIXELS)
#define W   (APP_IMAGE_WIDTH_PIXELS)

#define H_RAW   (MIPI_IMAGE_HEIGHT_PIXELS)
#define W_RAW   (MIPI_IMAGE_WIDTH_PIXELS)

// #define RAW_CAPTURE 0

#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

void camera_api_init();

void camera_api_new_row_raw(
    const int8_t pixel_data[H_RAW],
    const unsigned row_index);

void camera_api_new_row_decimated(
    const int8_t pixel_data[CH][W],
    const unsigned row_index);

unsigned camera_capture_row_raw(
    int8_t pixel_data[H_RAW]);

unsigned camera_capture_row_decimated(
    int8_t pixel_data[CH][W]);

unsigned camera_capture_image_raw(
    int8_t image_buff[H_RAW][W_RAW]);

unsigned camera_capture_image(
    int8_t image_buff[CH][H][W]);

#if defined(__XC__) || defined(__cplusplus)
}
#endif