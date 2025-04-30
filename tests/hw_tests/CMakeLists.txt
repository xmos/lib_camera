cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)

project(lib_camera_hw_tests)

add_subdirectory(test_auto_exposure)
add_subdirectory(test_capture_raw)
add_subdirectory(test_img_patterns)
add_subdirectory(test_rotate_90)
