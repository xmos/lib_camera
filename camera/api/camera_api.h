#pragma once

// xcore
// #include <xcore/channel_streaming.h>
#include "xccompat.h"
// user
#include "sensor.h"


#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

/**
 * SERVER SIDE
 * 
 * Initialize the camera API. Must be called before any other API functions.
 */
void camera_api_init();
void camera_stop();
unsigned camera_api_check_stop();

/**
 * SERVER SIDE
 * 
 * Called by the packet handler when a new row of raw image data is
 * available. Typically this will be Bayered image data.
 */
void camera_api_new_row_raw(
    const int8_t pixel_data[H_RAW],
    const unsigned row_index);

/**
 * SERVER SIDE
 * 
 * Called by the packet handler when a new row of decimated image data is
 * available.
 */
void camera_api_new_row_decimated(
    const int8_t pixel_data[CH][W],
    const unsigned row_index);

/**
 * CLIENT SIDE
 * 
 * Called by the client to capture a row of raw image data.
 * 
 * @param pixel_data The buffer to store the row of pixels in
 * 
 * @return The row index of the captured row of pixels
 */
unsigned camera_capture_row_raw(
    int8_t pixel_data[W_RAW]);

/**
 * CLIENT SIDE
 * 
 * Called by the client to capture a row of decimated image data.
 * 
 * @param pixel_data The buffer to store the row of pixels in
 * 
 * @return The row index of the captured row of pixels
 */
unsigned camera_capture_row_decimated(
    int8_t pixel_data[CH][W]);

/**
 * CLIENT SIDE
 * 
 * Called by the client to capture a raw image.
 * 
 * @param image_buff The buffer to store the image in
 * 
 * @return Returns 0 on success, non-zero on failure
 */
unsigned camera_capture_image_raw(
    int8_t image_buff[H_RAW][W_RAW]);


/**
 * CLIENT SIDE
 * 
 * Called by the client to capture a decimated image.
 * 
 * @param image_buff The buffer to store the image in
 * 
 * @return Returns 0 on success, non-zero on failure
 */
unsigned camera_capture_image(
    int8_t image_buff[CH][H][W]);

    
typedef struct {
  struct {
    unsigned row;
    unsigned col;
  } origin;
  struct {
    unsigned height;
    unsigned width;
  } shape;
} image_crop_params_t;

/**
 * CLIENT SIDE
 * 
 * Called by the client to capture a portion of a decimated image. If only a 
 * portion of the decimated image is required, using this function avoids the 
 * need to store the entire decimated image in memory.
 * 
 * `image_buff` must be a 3D array of 
 * size `[CH][crop_params.shape.height][crop_params.shape.width]`.
 * 
 * @param image_buff The buffer to store the image in
 * @param crop_params The parameters of the crop
 * 
 * @return Returns 0 on success, non-zero on failure
 */
unsigned camera_capture_image_cropped(
    int8_t* image_buff,
    const image_crop_params_t crop_params);

#if defined(__XC__) || defined(__cplusplus)
}
#endif
