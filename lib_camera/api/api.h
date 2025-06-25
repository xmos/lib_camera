// Copyright 2024-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

// Macros for data alignement
#ifndef ALIGNED_8
#define ALIGNED_8 __attribute__((aligned(8)))
#endif

#ifndef ALIGNED_4
#define ALIGNED_4 __attribute__((aligned(4)))
#endif

// Macro for C api definitions
#if defined(__cplusplus) || defined(__XC__)
#define C_API_START extern "C" {
#define C_API_END }
#else
#define C_API_START
#define C_API_END
#endif
