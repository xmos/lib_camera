#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "unity_fixture.h"

#include "_helpers.h"
#include "isp.h"            // gamma
#include "camera_utils.h"   // time

// Unity
TEST_GROUP(gamma_timing);
TEST_SETUP(gamma_timing) { fflush(stdout); print_separator("gamma_timing");}
TEST_TEAR_DOWN(gamma_timing) {}
TEST_GROUP_RUNNER(gamma_timing) {
    RUN_TEST_CASE(gamma_timing, gamma__basic);
    RUN_TEST_CASE(gamma_timing, gamma__double_size);
}

// Tests
void test_gamma_size(
    const char* func_name, 
    size_t height, 
    size_t width,
    size_t channels)
{
    uint8_t image_buffer[channels][height][width];

    // Seed the random number generator with the current time
    srand(time(NULL));

    // generate random numbers for the image buffer
    size_t buffsize = height * width * channels;
    fill_array_rand_uint8((uint8_t *) &image_buffer[0][0][0], buffsize);
    
    // then measure and apply gamma
    unsigned ts = measure_time();
    isp_gamma((uint8_t *) &image_buffer[0][0][0], gamma_new, height, width, channels);
    unsigned tdiff = measure_time() - ts;

    // print info
    printf("\tbuffsize: %d\n", buffsize);
    PRINT_NAME_TIME(func_name, tdiff);
}


TEST(gamma_timing, gamma__basic) 
{
    static const char func_name[] = "gamma downsampled";
    const size_t height = APP_IMAGE_HEIGHT_PIXELS;
    const size_t width = APP_IMAGE_WIDTH_PIXELS;
    const size_t channels = APP_IMAGE_CHANNEL_COUNT;
    test_gamma_size(func_name, height, width, channels);
}

TEST(gamma_timing, gamma__double_size) 
{
    static const char func_name[] = "gamma double size";
    const size_t height = APP_IMAGE_HEIGHT_PIXELS*2;
    const size_t width = APP_IMAGE_WIDTH_PIXELS*2;
    const size_t channels = APP_IMAGE_CHANNEL_COUNT;
    test_gamma_size(func_name, height, width, channels);
}
