// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include <print.h>
#include <xcore/hwtimer.h>

#include "api.h"

#define TO_MS(ticks) ((float)(ticks) / XS1_TIMER_KHZ)
#define PRINT_STR_MSG ">> ticks: "
#define PRINT_STR_MSG_MS ">> elapsed (ms): "

C_API_START

#define TIMEIT(func, ...) \
    { \
        unsigned start_time = get_reference_time(); \
        func(__VA_ARGS__); \
        unsigned end_time = get_reference_time(); \
        printstr(PRINT_STR_MSG); \
        printuintln(end_time - start_time); \
    }

#define TIMEIT_MS(func, ...) \
    { \
        unsigned start_time = get_reference_time(); \
        func(__VA_ARGS__); \
        unsigned end_time = get_reference_time(); \
        printstr(PRINT_STR_MSG_MS); \
        printuintln(((end_time - start_time)/XS1_TIMER_KHZ)); \
    }

void delay_ticks_cpp(unsigned ticks);
void delay_milliseconds_cpp(unsigned delay);
void delay_seconds_cpp(unsigned int delay);
void xmemcpy(
  void* dst,
  const void* src,
  unsigned bytes);

C_API_END
