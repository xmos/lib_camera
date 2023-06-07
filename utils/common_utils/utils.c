
#include "utils_new.h"
#include <stdint.h>

void write_image_new(char * filename, uint8_t * image, const size_t height, const size_t width)
{
    printf("enetered funtion\n");
    xscope_file_t fp = xscope_open_file(filename, "wb");
    printf("starting image write\n");

    for(size_t k = 0; k < height; k++)
    {
        for(size_t j = 0; j < width; j++)
        {
            size_t pos = k * width + j;
            xscope_fwrite(&fp, &image[pos], sizeof(uint8_t));
        }
    }

    //xscope_fwrite(&fp, image, height * width * sizeof(uint8_t));

    xscope_close_all_files();
    printf("Output file: %s\n", filename);
    printf("Image dimentions: %d x %d\n", width, height);
}

void c_memcpy(void * dst, void * src, size_t size)
{
    memcpy(dst, src, size);
}
