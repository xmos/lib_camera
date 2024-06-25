// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

// -------------------------- C --------------------------
#if defined(__XC__) || defined(__cplusplus)
extern "C" {
#endif

// Crop
void isp_crop_int8(
	int8_t* img,
	const unsigned in_width,
	const unsigned in_height,
	unsigned xu1,
	unsigned yu1,
	unsigned xu2,
	unsigned yu2);

void isp_crop_uint8(
	uint8_t* img,
	const unsigned in_width,
	const unsigned in_height,
	unsigned xu1,
	unsigned yu1,
	unsigned xu2,
	unsigned yu2);

// Resize
void isp_resize_int8(
	const int8_t* img,
	const unsigned in_width,
	const unsigned in_height,
	int8_t* out_img,
	const unsigned out_width,
	const unsigned out_height);

void isp_resize_uint8(
	const uint8_t* img,
	const unsigned in_width,
	const unsigned in_height,
	uint8_t* out_img,
	const unsigned out_width,
	const unsigned out_height);

void isp_rgb_to_greyscale(
	int8_t * gs_img,
	int8_t * img,
	unsigned n_pix);

#if defined(__XC__) || defined(__cplusplus)
} // extern "C"
#endif
