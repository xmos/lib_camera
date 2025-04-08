// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

// Test Static White Balance (SWB) 

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

#define MAX_BUFF_SIZE (640*480*1)

void test_wb(
	unsigned in_height,
	unsigned in_width,
	unsigned channels,
	const char* input_file,
	const char* output_file) 
{
	assert(channels == 3 && "only channel = 3 is supported");
	// Create Image Config
	int8_t image_buffer[MAX_BUFF_SIZE] ALIGNED_4 = { 0 };
	unsigned size = in_height * in_width * channels;
	camera_cfg_t config = {
		.offset_x = 0,
		.offset_y = 0,
		.mode = MODE_RGB1, // not relevant
	};
	image_cfg_t image = {
		.height = in_height,
		.width = in_width,
		.channels = channels,
		.size = size,
		.ptr = &image_buffer[0],
		.config = &config
	};

	// Compute the coordinates
	camera_isp_coordinates_compute(&image);

	// Read the image from file
	camera_io_read_file((char*)input_file, (uint8_t*)image.ptr, image.size);

	// Do the white balance
	unsigned t0 = get_reference_time();
	camera_isp_white_balance(&image);
	unsigned t1 = get_reference_time() - t0;
	float ops_per_px = (t1) / (float)image.size;

	// Write the image to file
	camera_io_write_file((char*)output_file, (uint8_t*)image.ptr, image.size);

	printf("Ops per pixel: %.2f \n", ops_per_px);
	printf("Time taken for white balance: %u \n", t1);
}

int main(int argc, char* argv[]) 
{
	assert(argc == 6);
	unsigned height = atoi(argv[1]);
	unsigned width = atoi(argv[2]);
	unsigned channels = atoi(argv[3]);
	const char* input_file = argv[4];
	const char* output_file = argv[5];
	printf("--------------------\n");
	printf("Image size: %dx%d\n", width, height);
	test_wb(height, width, channels, input_file, output_file);
	return 0;
}
