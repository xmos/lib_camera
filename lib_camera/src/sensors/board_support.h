// Copyright 2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

// Define only one board type
#define BOARD_XU316_AIV 0
#define BOARD_XU316_EXP 1

// Select which board to use
#define BOARD_SUPPORT BOARD_XU316_EXP

// Board specific definitions
#if (BOARD_SUPPORT == BOARD_XU316_AIV)
#define  SCL_BIT_POS  0x1
#define  SCL_BIT_MASK 0xC
#define  SDA_BIT_POS  0x0
#define  SDA_BIT_MASK 0xC
#elif (BOARD_SUPPORT == BOARD_XU316_EXP)
#define  SCL_BIT_POS  0x0
#define  SCL_BIT_MASK 0x0
#define  SDA_BIT_POS  0x0
#define  SDA_BIT_MASK 0x0
#define  PORT_I2C_SCL XS1_PORT_1N
#define  PORT_I2C_SDA XS1_PORT_1O
#else
#error "Board support not defined or not supported"
#endif
