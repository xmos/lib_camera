// Copyright 2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

// Macro for C api definitions
#if defined(__cplusplus) || defined(__XC__)
#define C_API_START extern "C" {
#define C_API_END }
#else
#define C_API_START
#define C_API_END
#endif
