// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

// Runs RAW8 to YUV conversion from ISP module

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <xcore/hwtimer.h>

#include "camera.h"
#include "camera_isp.h"
#include "camera_io.h"

#define MAX_BUFF_SIZE (640*480*1)
#define TO_MS(ticks) ((float)(ticks) / XS1_TIMER_KHZ)

void test_yuv(
	unsigned in_height,
	unsigned in_width,
	unsigned channels,
	const char* input_file,
	const char* output_file) 
{
	assert(channels == 2 && "only channel = 2 is supported");
	
	camera_mode_t mode = MODE_YUV2;
	const unsigned out_h = in_height >> 1; // YUV422 downsamples by 2 vertically (only from raw8)
	const unsigned out_w = in_width >> 1; // YUV422 downsampled by 2 horizontally (always)
	const unsigned out_ch = 2;
	const unsigned out_size = out_h * out_w * out_ch;
	const unsigned in_size = in_height * in_width * 1;
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
		.ptr = image_buffer,
		.config = &config
	};

	// Compute the coordinates
	camera_isp_coordinates_compute(&image);

	// Read the raw image from file
	// send it row by row
	FILE* fp = fopen(input_file, "rb");
	assert(fp != NULL);

	int8_t* img_row = (int8_t*)malloc(in_width);
	for (int i = 0; i < in_height; i++) {
		fread((uint8_t*)&img_row[0], 1, in_width, fp);
		ta = get_reference_time();
		camera_isp_raw8_to_yuv2(&image, img_row, i);
		tb += get_reference_time() - ta;
	}

	fclose(fp);
	free(img_row);

	// Write the image to file
	printf("Writing image to file\n");
	camera_io_write_file(
		(char*)output_file, 
		(uint8_t*)image.ptr, 
		image.size
	);

    // Print measurements
    float ops_out_per_pixel = (float)tb / out_size;
	float ops_in_per_pixel = (float)tb / in_size;
	printf("Ops per <in> pixel:\t%.3f\n", ops_in_per_pixel);
	printf("Ops per <out> pixel:\t%.3f\n", ops_out_per_pixel);
	printf("Avg Time per row (ms):\t%.3f\n", TO_MS(tb / image.height));
	printf("Tot Time per img (ms):\t%.3f\n", TO_MS(tb));
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
	test_yuv(height, width, channels, input_file, output_file);
	return 0;
}
