#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#include <timer.h>

#include "unity_fixture.h"

#include "_helpers.h"
#include "statistics.h"      
#include "camera_utils.h"   // time

// Unity
TEST_GROUP(stats_test);
TEST_SETUP(stats_test) { fflush(stdout); print_separator("stats_test");}
TEST_TEAR_DOWN(stats_test) {}
TEST_GROUP_RUNNER(stats_test) {
    RUN_TEST_CASE(stats_test, stats_test__basic);
}


TEST(stats_test, stats_test__basic){
    // create a random array
    const size_t height = APP_IMAGE_HEIGHT_PIXELS / K;
    const size_t width = APP_IMAGE_WIDTH_PIXELS / K;
    const size_t channels = APP_IMAGE_CHANNEL_COUNT;
    const size_t buffsize = height * width * channels;

    int8_t image_buffer[channels][height][width];
    fill_array_rand_int8((int8_t *) &image_buffer[0][0][0], buffsize);

    // create empty stats
    global_stats_t global_stats = {{0}};

    // compute histogram
    unsigned ts = measure_time();
    unsigned tdiff_internal = 0;

    for (uint16_t h=0; h<height; h++){
        unsigned ts_internal = measure_time();
        for (uint8_t channel = 0; channel < APP_IMAGE_CHANNEL_COUNT; channel++) {
            stats_update_histogram(
            &global_stats[channel].histogram, 
            &image_buffer[channel][h][0]);
        }
        tdiff_internal = measure_time() - ts_internal;
    }
    unsigned tdiff = measure_time() - ts;
    fast_print_name_time("time per histogram (row)", tdiff_internal);
    fast_print_name_time("time per histogram (all)", tdiff);

    // End of frame, compute statistics (order is important)
    unsigned int ts0, ts1, ts2, ts3, ts4, total_time;
    uint8_t channel = 0;
    ts0 = measure_time();
    stats_simple(&global_stats[channel]);
    ts1 = measure_time();
    stats_skewness(&global_stats[channel]);
    ts2 = measure_time();
    stats_percentile(&global_stats[channel], APP_WB_PERCENTILE);
    ts3 = measure_time();
    stats_percentile_volume(&global_stats[channel]);
    ts4 = measure_time();

    total_time = ts4 - ts0;
    ts0 = ts1 - ts0;
    ts1 = ts2 - ts1;
    ts2 = ts3 - ts2;
    ts3 = ts4 - ts3;

    fast_print_name_time("time per stats_simple", ts0);
    fast_print_name_time("time per stats_skewness", ts1);
    fast_print_name_time("time per stats_percentile", ts2);
    fast_print_name_time("time per stats_percentile_volume", ts3);
    fast_print_name_time("time total", total_time);

    stats_print(&global_stats[channel], channel);

    // timing per channel has to meet time between frames 1/30s = 33ms
    // time per stats*3 channels should be lower than 33ms 
    TEST_ASSERT_LESS_THAN_FLOAT(33 , total_time*3*0.00001);
}
