// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>

#include <debug_print.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"

void camera_isp_raw8_to_raw8(image_cfg_t* image, int8_t* data_in, unsigned ln) {
  unsigned img_ln = ln - image->config->y1;
  int8_t* data_src = data_in + image->config->x1;
  int8_t* data_dst = image->ptr + (img_ln * image->width);
  xmemcpy(
    data_dst,
    data_src,
    image->width);
}
