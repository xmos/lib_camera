// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <xs1.h>
#include <platform.h>
#include <xccompat.h>

#include "camera_main.h"

#ifdef __XC__
typedef chanend chanend_t;
#endif

#define MIPI_TILE  1

#ifndef MIPI_CLKBLK
# define MIPI_CLKBLK                  XS1_CLKBLK_1
#endif

void mipi_main(chanend_t c_control);
