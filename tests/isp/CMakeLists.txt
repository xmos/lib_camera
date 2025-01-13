cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_isp_rgb1)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)
message("XMOS_SANDBOX_DIR: ${XMOS_SANDBOX_DIR}")

# Includes and dependancies
list(APPEND APP_INCLUDES src)
list(APPEND APP_DEPENDENT_MODULES "lib_camera")

# Target, Xscope
set(APP_XSCOPE_SRCS src/config.xscope)
set(APP_HW_TARGET src/XCORE-VISION-EXPLORER.xn)

# options
option(FILE_IN_NAME "Pass FILE_IN_NAME value" OFF)
option(FILE_OUT_NAME "Pass FILE_OUT_NAME value" OFF)
option(USE_OPTIMISED "Use optimised version of the code" OFF)

# flags
set(
    APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -fxscope
)

if (FILE_IN_NAME)
list(APPEND APP_COMPILER_FLAGS -DFILE_IN_NAME="${FILE_IN_NAME}")
endif()

if (FILE_OUT_NAME)
list(APPEND APP_COMPILER_FLAGS -DFILE_OUT_NAME="${FILE_OUT_NAME}")
endif()

if (USE_OPTIMISED)
list(APPEND APP_COMPILER_FLAGS -DUSE_OPTIMISED=1)
endif()

XMOS_REGISTER_APP()
