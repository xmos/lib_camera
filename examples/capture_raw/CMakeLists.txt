cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(capture_raw)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)
message("XMOS_SANDBOX_DIR: ${XMOS_SANDBOX_DIR}")

# Target, xscope
set(APP_HW_TARGET src/XK-EVK-XU316-AIV.xn)
set(APP_XSCOPE_SRCS src/config.xscope)

# Includes, Dependencies
list(APPEND APP_INCLUDES src)
list(APPEND APP_DEPENDENT_MODULES "lib_camera")

# Flags
set(
    APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -DDEBUG_PRINT_ENABLE_CAM_ISP=1 # Enable debug prints
)

XMOS_REGISTER_APP()
