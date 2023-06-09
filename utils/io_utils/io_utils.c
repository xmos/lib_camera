
#include "io_utils.h"
#include <stdint.h>

void swap_dimentions(uint8_t * image_in, uint8_t * image_out, const size_t height, const size_t width, const size_t channels)
{
    printf("Swapping image dimentions\n");
    for(size_t k = 0; k < height; k++)
    {
        for(size_t j = 0; j < width; j++)
        {
            for(size_t c = 0; c < channels; c++)
            {
                size_t index_in = c * (height * width) + k * width  + j - 1;
                size_t index_out = k * (width * channels) + j * channels + c - 1;
                image_out[index_out] = image_in[index_in];
            }
        }
    }
}

void write_file(char * filename, uint8_t * data, const size_t size)
{
    xscope_file_t fp = xscope_open_file(filename, "wb");

    xscope_fwrite(&fp, data, size);

    xscope_close_all_files();
}

void write_image_file(char * filename, uint8_t * image, const size_t height, const size_t width, const size_t channels)
{
    printf("Writing image...\n");

    const size_t img_size = height * width * channels * sizeof(uint8_t);
    write_file(filename, image, img_size);

    printf("Image written into file: %s\n", filename);
    printf("Image dimentions: %d x %d\n", width, height);
}
