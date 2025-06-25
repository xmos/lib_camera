cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

project(lib_camera_tests)

add_subdirectory(hw_tests)
add_subdirectory(isp)
add_subdirectory(unit_tests)
