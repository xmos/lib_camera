cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_camera)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_INCLUDES api)

set(CONFIG_RAW8 0x2A)
set(CONFIG_RAW10 0x2B)
set(CONFIG_MIPI_FORMAT ${CONFIG_RAW8})

set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -Wno-xcore-fptrgroup
    -mcmodel=large)

set(APP_XSCOPE_SRCS config.xscope)

set(APP_DEPENDENT_MODULES camera "danielpieczko/lib_unity")

set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${FWK_CAMERA_ROOT_DIR}/common.cmake)

XMOS_REGISTER_APP()
