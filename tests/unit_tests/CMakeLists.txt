cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(test_camera)

# sandbox
set(XMOS_SANDBOX_DIR ${CMAKE_SOURCE_DIR}/../../..)

# target
set(APP_HW_TARGET XCORE-AI-EXPLORER)
list(APPEND APP_INCLUDES api src src/test)
list(APPEND APP_C_SRCS
    src/main.c
    src/test/_helpers.c
    src/test/color_conversion_test.c
    #src/test/gamma_timing_test.c
    #src/test/pixel_hfilter_test.c
    #src/test/pixel_vfilter_test.c
    #src/test/statistics_test.c
    #src/test/resize_function_test.c
    #src/test/crop_function_test.c
)
list(APPEND APP_DEPENDENT_MODULES lib_camera "lib_unity(2.5.2)" "xscope_fileio(1.2.0)")

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
    -mcmodel=large
    -Wno-xcore-fptrgroup)

XMOS_REGISTER_APP()
