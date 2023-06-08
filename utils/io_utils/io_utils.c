
#include "io_utils.h"
#include <stdint.h>

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
