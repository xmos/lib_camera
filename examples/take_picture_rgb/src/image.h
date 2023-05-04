#include <stdint.h>

#define BYTES_PER_PIXEL 4
#define LEN(arr)    ((int) (sizeof (arr) / sizeof (arr)[0]))
#define CHEIGHT(X)  LEN(X)
#define CWIDTH(X)   LEN(X[0])

void reduce_size2(int width, int height, uint8_t in_array[ ][width], int width_out,  int height_out, uint8_t out_array[ ][width_out])
{
    uint16_t count = 0;
    for(uint16_t  j = 0; j < height - 1; j+=2) {
        for(uint16_t i = 0; i < width - BYTES_PER_PIXEL; i+=2*BYTES_PER_PIXEL) {
            for (char c=0; c < BYTES_PER_PIXEL; c++){
                int pix = (
                    in_array[j][i+c] + // first red
                    in_array[j][i+BYTES_PER_PIXEL+c] + // second red, same row
                    in_array[j+1][i+c] + // red of same col one more row
                    in_array[j+1][i+BYTES_PER_PIXEL+c]   // red of col and row + 1
                    )/4;
                out_array[j/2][count++] = pix;
            }
        }
        count = 0;
    }
}
