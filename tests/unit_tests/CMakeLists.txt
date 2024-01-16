cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_camera)

# root
set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/../..)
include(${FWK_CAMERA_ROOT_DIR}/build_opts.cmake)

# target
set(APP_HW_TARGET XCORE-AI-EXPLORER)
list(APPEND APP_INCLUDES api src src/test)
list(APPEND APP_C_SRCS
    src/main.c
    src/test/_helpers.c
    src/test/color_conversion_test.c
    src/test/gamma_timing_test.c
    src/test/pixel_hfilter_test.c
    src/test/pixel_vfilter_test.c
    src/test/statistics_test.c
)
list(APPEND APP_DEPENDENT_MODULES ${lib_camera} ${Unity})

# common
set(COM_DIR ../../examples/common)
list(APPEND APP_INCLUDES ${COM_DIR})
list(APPEND APP_C_SRCS ${COM_DIR}/camera_io_utils.c)
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
    -mcmodel=large
    -Wno-xcore-fptrgroup)

XMOS_REGISTER_APP()
