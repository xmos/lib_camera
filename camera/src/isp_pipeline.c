#include <xcore/channel.h> // includes streaming channel and channend
#include <xcore/channel_streaming.h>
#include "xccompat.h"

#include "isp.h"
#include "statistics.h"

/**
 * Thread that computes the ISP pipeline for each pixel in the image.
 * The statistics are stored in a struct which is used to perform ISP corrections.
 * @param c_img_in - Channel end of the image.
 */
void isp_pipeline(streaming_chanend_t c_img_in, chanend_t schan[])
{
    // Outer loop iterates over frames
    while (1) {
        global_stats_t global_stats = {{0}};

        // Inner loop iterates over rows within a frame
        while (1) {
            low_res_image_row_t* row = (low_res_image_row_t*) s_chan_in_word(c_img_in);

            if (row == NULL) { // Signal end of frame [1]
                break;
            }

            if (row == (low_res_image_row_t *) 1) {
                // Stop the camera sensor
                chan_out_word(schan[SENSOR_STREAM_STOP], (uint32_t)0);
                uint32_t r = chan_in_word(schan[SENSOR_STREAM_STOP]);
                printf("ISP: Received stop signal response %ld\n", r);
                if (r == 0) return;
            }

            // Update histogram
            for (uint8_t channel = 0; channel < APP_IMAGE_CHANNEL_COUNT; channel++) {
                stats_update_histogram(
                  &global_stats[channel].histogram, 
                  &row->pixels[channel][0]);
            }
        }

        // End of frame, compute statistics (order is important)
        for (uint8_t channel = 0; channel < APP_IMAGE_CHANNEL_COUNT; channel++) {
            stats_simple(&global_stats[channel]);
            stats_skewness(&global_stats[channel]);
            stats_percentile(&global_stats[channel], APP_WB_PERCENTILE);
            stats_percentile_volume(&global_stats[channel]);
            #if ENABLE_PRINT_STATS
                stats_print(&global_stats[channel], channel); // print channel stats
            #endif
        }

        // Adjust AE
        uint8_t ae_done = AE_control_exposure(&global_stats, schan[SENSOR_SET_EXPOSURE]);

        // Adjust AWB
        static unsigned run_once = 0;
        if (ae_done == 1 && run_once == 0) 
        {
            AWB_compute_gains_static(&isp_params);
            run_once = 1; // Set to 1 to run only once
        }

        // Apply gamma curve
        // gamma curve is applied to the image in the camera API

        // Print ISP info
        #if ENABLE_PRINT_STATS
            AWB_print_gains(&isp_params);
        #endif
    }
}
