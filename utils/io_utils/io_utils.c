
#include "io_utils.h"
#include <stdint.h>

void write_image_file(char * filename, uint8_t * image, const size_t size, const size_t height, const size_t width)
{
    xscope_file_t fp = xscope_open_file(filename, "wb");

    printf("Writing image...\n");
    xscope_fwrite(&fp, image, size);

    xscope_close_all_files();
    printf("Image written into file: %s\n", filename);
    printf("Image dimentions: %d x %d\n", width, height);
}
