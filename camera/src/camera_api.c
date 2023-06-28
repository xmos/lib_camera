// std
#include <stdio.h>
#include <string.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel_streaming.h>
// user
#include "mipi.h"
#include "camera_utils.h"
#include "camera_api.h"

#define CHAN_RAW 0
#define CHAN_DEC 1
#define CHAN_STOP 2

// In order to interface the handler and api
streaming_channel_t c_user_api[3];

void camera_init()
{
  c_user_api[CHAN_RAW]   = s_chan_alloc();
  c_user_api[CHAN_DEC]   = s_chan_alloc();
  c_user_api[CHAN_STOP]  = s_chan_alloc();
}

unsigned camera_check_stop(){
  SELECT_RES(
      CASE_THEN(c_user_api[CHAN_STOP].end_b, user_handler),
      DEFAULT_THEN(default_handler))
    {
      user_handler:
        return 1;
      default_handler:
        return 0;
    }
}

void camera_stop(){
  s_chan_out_word(c_user_api[CHAN_STOP].end_a, (unsigned) 1);
}

void camera_new_row(
    const int8_t pixel_data[W_RAW],
    const unsigned row_index)
{
  int8_t* user_pixel_data;

  SELECT_RES(
      CASE_THEN(c_user_api[CHAN_RAW].end_a, user_handler),
      DEFAULT_THEN(default_handler))
    {
      user_handler:
        user_pixel_data = (int8_t*) s_chan_in_word(c_user_api[CHAN_RAW].end_a);
        memcpy(user_pixel_data, (void*) pixel_data, W_RAW);
        s_chan_out_word(c_user_api[CHAN_RAW].end_a, row_index);
        break;
      default_handler:
        break;
    }
}

void camera_new_row_decimated(
    const int8_t pixel_data[CH][W],
    const unsigned row_index)
{
  int8_t* user_pixel_data;

  SELECT_RES(
      CASE_THEN(c_user_api[CHAN_DEC].end_a, user_handler),
      DEFAULT_THEN(default_handler))
    {
      user_handler:
        user_pixel_data = (int8_t*) s_chan_in_word(c_user_api[CHAN_DEC].end_a);
        memcpy(user_pixel_data, (void*) pixel_data, CH*W);
        s_chan_out_word(c_user_api[CHAN_DEC].end_a, row_index);
        break;
      default_handler:
        break;
    }
}

unsigned camera_capture_row(
    int8_t pixel_data[W_RAW])
{
  s_chan_out_word(c_user_api[CHAN_RAW].end_b, (unsigned) &pixel_data[0]);
  unsigned sdf = s_chan_in_word(c_user_api[CHAN_RAW].end_b);
  return sdf;
}

unsigned camera_capture_row_decimated(
    int8_t pixel_data[CH][W])
{
  s_chan_out_word(c_user_api[CHAN_DEC].end_b, (unsigned) &pixel_data[0][0]);
  return s_chan_in_word(c_user_api[CHAN_DEC].end_b);
}

unsigned camera_capture_image_raw(
    int8_t image_buff[H_RAW][W_RAW])
{
  unsigned row_index;

  // Loop, capturing rows until we get one with row_index==0
  do {
    row_index = camera_capture_row(&image_buff[0][0]);
  } while (row_index != 0);

  // Now capture the rest of the rows
  for (unsigned i=1; i<H_RAW; i++) {
    row_index = camera_capture_row(&image_buff[i][0]);
    if (row_index != i) {
      return 1; // TODO handle errors better
    }
  }

  return 0;
}

unsigned camera_capture_image_transpose(
    int8_t image_buff[CH][H][W])
{
  unsigned row_index;

  int8_t pixel_data[CH][W];

  // Loop, capturing rows until we get one with row_index==0
  do {
    row_index = camera_capture_row_decimated(pixel_data);
  } while (row_index != 0);

  for(int c = 0; c < CH; c++) 
    memcpy(&image_buff[c][0][0], &pixel_data[c][0], W);

  // Now capture the rest of the rows
  for (unsigned row = 1; row < H; row++) {
    row_index = camera_capture_row_decimated(pixel_data);

    if (row_index != row)      return 1; // TODO handle errors better

    for(int c = 0; c < CH; c++)
      memcpy(&image_buff[c][row][0], &pixel_data[c][0], W);
      
  }

  return 0;
}

unsigned camera_capture_image(
    int8_t image_buff[H][W][CH])
{
  unsigned row_index;

  int8_t pixel_data[CH][W];

  // Loop, capturing rows until we get one with row_index==0
  do {
    row_index = camera_capture_row_decimated(pixel_data);
  } while (row_index != 0);

  for(int i = 0; i < W; i++)
    for(int c = 0; c < CH; c++)
      image_buff[0][i][c] = pixel_data[c][i];
  
  // Now capture the rest of the rows
  for (unsigned row = 1; row < H; row++) {
    row_index = camera_capture_row_decimated(pixel_data);

    if (row_index != row)      return 1; // TODO handle errors better

    for(int i = 0; i < W; i++)
      for(int c = 0; c < CH; c++)
        image_buff[row][i][c] = pixel_data[c][i];

  }

  return 0;
}

unsigned camera_capture_image_cropped(
    int8_t* image_buff,
    const image_crop_params_t crop_params)
{
  const unsigned CROP_ROW = crop_params.origin.row;
  const unsigned CROP_COL = crop_params.origin.col;
  const unsigned CROP_H = crop_params.shape.height;
  const unsigned CROP_W = crop_params.shape.width;

  unsigned row_index;

  int8_t pixel_data[CH][W];

  int8_t (*image)[CROP_H][CROP_W] = 
    (int8_t (*)[CROP_H][CROP_W]) image_buff;

  // Loop, capturing rows until we get one 
  // with row_index==crop_params.origin.row
  do {
    row_index = camera_capture_row_decimated(pixel_data);
  } while (row_index != CROP_ROW);

  for(int c = 0; c < CH; c++) 
    memcpy(&image[c][0][0], &pixel_data[c][CROP_COL], CROP_W);

  // Now capture the rest of the rows
  for (unsigned row = 1; row < CROP_H; row++) {
    row_index = camera_capture_row_decimated(pixel_data);
    
    // TODO handle errors better
    if (row_index != row + crop_params.origin.row)  return 1; 

    for(int c = 0; c < CH; c++)
      memcpy(&image[c][row][0], &pixel_data[c][CROP_COL], CROP_W);
      
  }

  return 0;
}
