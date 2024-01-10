cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(take_picture_downsample)

set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${FWK_CAMERA_ROOT_DIR}/common.cmake)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_INCLUDES src)

set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -mcmodel=large)

set(APP_XSCOPE_SRCS ../config.xscope)
set(APP_DEPENDENT_MODULES ${lib_camera} ${xscope_fileio})

XMOS_REGISTER_APP()
