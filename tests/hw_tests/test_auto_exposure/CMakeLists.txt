cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(capture_rgb)

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
    INIT_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -DDEBUG_PRINT_ENABLE_CAM_ISP=1  # ISP debug prints
    -DDEBUG_PRINT_ENABLE_CAM_MIPI=1 # MIPI debug prints
)

# ----- AE/AWB OFF -----
project(test_awb_ae_off)
set(
    APP_COMPILER_FLAGS
    ${INIT_COMPILER_FLAGS}
    -DCONFIG_APPLY_AWB=0
    -DCONFIG_APPLY_AE=0
)
XMOS_REGISTER_APP()

# ----- AE OFF -----
project(test_awb_on_ae_off)
set(
    APP_COMPILER_FLAGS
    ${INIT_COMPILER_FLAGS}
    -DCONFIG_APPLY_AWB=1
    -DCONFIG_APPLY_AE=0
)
XMOS_REGISTER_APP()

# ----- AE ON -----
project(test_awb_ae_on)
set(
    APP_COMPILER_FLAGS
    ${INIT_COMPILER_FLAGS}
    -DCONFIG_APPLY_AWB=1
    -DCONFIG_APPLY_AE=1
)
XMOS_REGISTER_APP()
