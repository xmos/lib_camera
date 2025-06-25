// Copyright 2020-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#define UNITY_SUPPORT_64 1
#define UNITY_INCLUDE_DOUBLE 1

// Prevents Unity from taking control of malloc() and free()
#define UNITY_FIXTURE_NO_EXTRAS  (1)