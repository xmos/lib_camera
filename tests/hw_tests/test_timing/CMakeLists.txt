cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_timing)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../../..)
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
    -fxscope
    -DDEBUG_PRINT_ENABLE_CAM_ISP=1  # ISP debug prints
    -DDEBUG_PRINT_ENABLE_CAM_MIPI=1 # MIPI debug prints
    -DCONFIG_APPLY_AE=0 # Disable AE for this test
)

XMOS_REGISTER_APP()
