// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <xscope.h>
#include <xcore/hwtimer.h>
#include <xcore/assert.h>
#include <xcore/parallel.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_utils.h"
#include "camera_io.h"
#include "camera_conv.h"

#define TO_MS(ticks) ((float)(ticks) / XS1_TIMER_KHZ)
#define MAX_BUFF_SIZE (640*480*1)

static
void camera_isp_to_rgbx(
	image_cfg_t* image,
	int8_t* data_in,
	unsigned sensor_ln,
	camera_mode_t mode)
{
	switch (mode) {
	case MODE_RAW: {
		camera_isp_raw8_to_raw8(image, data_in, sensor_ln); break;
	}
	case MODE_RGB1: {
		camera_isp_raw8_to_rgb1(image, data_in, sensor_ln); break;
	}
	case MODE_RGB2: {
		camera_isp_raw8_to_rgb2(image, data_in, sensor_ln); break;
	}
	case MODE_RGB4: {
		camera_isp_raw8_to_rgb4(image, data_in, sensor_ln); break;
	}
	default: {
		xassert(0 && "mode not supported"); break;
	}
	}
}

void test_isp(
	unsigned in_height,
	unsigned in_width,
	unsigned ds_factor,
	const char* input_file,
	const char* output_file)
{
	camera_mode_t mode = (camera_mode_t)ds_factor;
	unsigned out_h = in_height >> (ds_factor - 1);
	unsigned out_w = in_width >> (ds_factor - 1);
	unsigned out_ch = 3;
	unsigned out_size = out_h * out_w * out_ch;

	void(*isp_fp)(image_cfg_t*, int8_t*, unsigned);
	isp_fp = camera_isp_raw8_to_rgb1;

	// Time variables
	unsigned ta = 0, tb = 0;

	// Create a Configuration
	int8_t image_buffer[MAX_BUFF_SIZE] ALIGNED_8 = { 0 };

	camera_cfg_t config = {
		.offset_x = 0,
		.offset_y = 0,
		.mode = mode,
	};
	image_cfg_t image = {
		.height = out_h,
		.width = out_w,
		.channels = out_ch,
		.size = out_size,
		.ptr = &image_buffer[0],
		.config = &config
	};
	camera_isp_coordinates_compute(&image);

	// Read the raw image from file
	// send it row by row
	FILE* fp = fopen(input_file, "rb");
	assert(fp != NULL);
	// allocate with malloc
	int8_t* img_row = (int8_t*)malloc(in_width);
	memset(img_row, 0, in_width);
	for (int i = 0; i < in_height; i++) {
		fread((uint8_t*)&img_row[0], 1, in_width, fp);
		ta = get_reference_time();
		camera_isp_to_rgbx(&image, img_row, i, mode);
		tb += get_reference_time() - ta;
	}
	float ops_per_pixel = (float)tb / image.size;
	printf("Average time per row (ms): %f\n", TO_MS(tb / image.height));
	printf("Total time (ms): %f\n", TO_MS(tb));
	printf("Ops per pixel: %.2f\n", ops_per_pixel);
	fclose(fp);
	free(img_row);
	// Write the image to file
	printf("Writing image to file\n");
	camera_io_write_file((char*)output_file, (uint8_t*)image.ptr, image.size);
}

int main(int argc, char* argv[])
{
	assert(argc == 6);
	unsigned height = atoi(argv[1]);
	unsigned width = atoi(argv[2]);
	unsigned ds_factor = atoi(argv[3]);
	const char* input_file = argv[4];
	const char* output_file = argv[5];
	printf("--------------------\n");
	printf("Image size: %dx%d\n", width, height);
	printf("Downsampling factor: %d\n", ds_factor);
	test_isp(height, width, ds_factor, input_file, output_file);
}
