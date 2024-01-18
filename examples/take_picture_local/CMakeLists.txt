cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(take_picture_local)

# root
set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${FWK_CAMERA_ROOT_DIR}/build_opts.cmake)

# target
set(APP_HW_TARGET XCORE-AI-EXPLORER)
list(APPEND APP_INCLUDES src)
list(APPEND APP_C_SRCS src/app.c)
list(APPEND APP_XC_SRCS src/main.xc)
list(APPEND APP_DEPENDENT_MODULES lib_camera)

# common
set(COM_DIR ../common)
list(APPEND APP_INCLUDES ${COM_DIR})
list(APPEND APP_C_SRCS ${COM_DIR}/camera_io_utils.c ${COM_DIR}/packet_rx_simulate.c)
list(APPEND APP_DEPENDENT_MODULES ${xscope_fileio})

# xscope
set(APP_XSCOPE_SRCS ../config.xscope)

# flags
set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -mcmodel=large)

XMOS_REGISTER_APP()
