cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(capture_raw)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)
message("XMOS_SANDBOX_DIR: ${XMOS_SANDBOX_DIR}")

# target
set(APP_HW_TARGET XCORE-VISION-EXPLORER.xn)
list(APPEND APP_INCLUDES src)
list(APPEND APP_DEPENDENT_MODULES "lib_camera" "xscope_fileio(1.2.0)")

# xscope
set(APP_XSCOPE_SRCS config.xscope)

# flags
set(APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -mcmodel=large)

XMOS_REGISTER_APP()
