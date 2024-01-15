cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_camera)
set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${FWK_CAMERA_ROOT_DIR}/lib_camera.cmake)

set(APP_HW_TARGET XCORE-AI-EXPLORER)
set(APP_INCLUDES api)

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
set(APP_DEPENDENT_MODULES ${lib_camera} ${Unity})

# xscope
set(COM_DIR ../../examples/common)
list(APPEND APP_INCLUDES ${COM_DIR})
list(APPEND APP_C_SRCS ${COM_DIR}/camera_io_utils.c)
list(APPEND APP_DEPENDENT_MODULES ${xscope_fileio})

XMOS_REGISTER_APP()
