cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

project(lib_camera_examples)

add_subdirectory(capture_raw)
add_subdirectory(capture_rgb)
