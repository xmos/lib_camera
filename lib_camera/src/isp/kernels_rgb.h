// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

// Mask for storing results
#define KERNEL_MASK 0x1FFF


extern const int16_t kernel_shifts[16];

extern const int8_t kernels12[];
extern const int8_t * const kernels_demosaic_12[48];
