# ----- Common configuration -----

# Cmake and Sandbox
cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)
message("XMOS_SANDBOX_DIR: ${XMOS_SANDBOX_DIR}")

# Target
set(APP_HW_TARGET src/XCORE-VISION-EXPLORER.xn)

# flags
set(
    APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -fxscope
    -DDEBUG_PRINT_ENABLE_CAM_ISP=1
)

# ----- RGB1 -----
project(test_isp_rgb1)
set(APP_INCLUDES src)
set(APP_C_SRCS src/test_rgb1.c)
set(APP_DEPENDENT_MODULES "lib_camera")
XMOS_REGISTER_APP()

# ----- RGB2 -----
project(test_isp_rgb2)
set(APP_INCLUDES src)
set(APP_C_SRCS src/test_rgb2.c)
set(APP_DEPENDENT_MODULES "lib_camera")
XMOS_REGISTER_APP()
