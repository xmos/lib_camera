
extern "C" {
#include <xs1.h>
#include <stdio.h>
#include <stdlib.h>
#include <platform.h>
#include <xcore/channel.h>
#include <xcore/port.h>
#include <xcore/parallel.h>
#include <xcore/hwtimer.h>
#include <xcore/select.h>
#include <xscope.h>
#include <platform.h>
#include <xcore/triggerable.h>
#include "i2c.h"
#include "i2c_reg.h"
}

#ifndef MIPI_TILE
    #define MIPI_TILE 1
#endif

#define I2C_SPEED 400

uint32_t p_scl_bit_pos = 0;
uint32_t p_sda_bit_pos = 0;

#define SETSR(c) asm volatile("setsr %0" \
                              :          \
                              : "n"(c));
