
#include "utils.h"
#include <stdint.h>

void write_image(char * filename, uint8_t * image, const size_t height, const size_t width)
{
    xscope_file_t fp = xscope_open_file(filename, "wb");

    xscope_fwrite(&fp, image, height * width * sizeof(uint8_t));

    xscope_close_all_files();
    printf("Output file: %s\n", filename);
    printf("Image dimentions: %d x %d\n", width, height);
}

void not_silly_memcpy(void * dst, void * src, size_t size)
{
    memcpy(dst, src, size);
}
