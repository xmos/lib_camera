#ifndef MIPI_TILE
    #define MIPI_TILE 1
#endif

#define I2C_SPEED 400

uint32_t p_scl_bit_pos = 0;
uint32_t p_sda_bit_pos = 0;

#define SETSR(c) asm volatile("setsr %0" \
                              :          \
                              : "n"(c));
