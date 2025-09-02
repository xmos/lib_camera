cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(unit_tests)

# sandbox
set(XMOS_SANDBOX_DIR ${CMAKE_CURRENT_LIST_DIR}/../../..)

# target
# set(APP_HW_TARGET src/XK-EVK-XU316-AIV.xn)
set(APP_HW_TARGET XK-EVK-XU316)

# sources
list(APPEND APP_INCLUDES src src/test)

list(APPEND APP_DEPENDENT_MODULES 
    lib_camera 
    "lib_unity(2.5.2)"
)

# flags
set(APP_COMPILER_FLAGS
    -O3
    -g
    -report
    -Wall
    -Werror
    #-fxscope
    -mcmodel=large
    -Wno-xcore-fptrgroup)

XMOS_REGISTER_APP()
