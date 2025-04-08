# Cmake and Sandbox
cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)

# Target
set(APP_HW_TARGET XK-EVK-XU316) # xsim

# flags
set(
    APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -fxscope
    -DDEBUG_PRINT_ENABLE_CAM_ISP=1
    -fcmdline-buffer-bytes=1024
    -DLIBXCORE_XASSERT_IS_ASSERT 
)

# ----- RGBX -----
project(test_isp_rgb)
set(APP_INCLUDES src)
set(APP_C_SRCS src/test_rgb.c)
set(APP_DEPENDENT_MODULES "lib_camera")
XMOS_REGISTER_APP()

# ----- SWB -----
project(test_isp_wb)
set(APP_INCLUDES src)
set(APP_C_SRCS src/test_swb.c)
set(APP_DEPENDENT_MODULES "lib_camera")
XMOS_REGISTER_APP()
