
include(xscope_fileio.cmake)

set(LIB_NAME fwk_camera_utils)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} INTERFACE io_utils/io_utils.c)

target_include_directories(${LIB_NAME} INTERFACE io_utils)

add_library(fwk_camera::utils ALIAS ${LIB_NAME})
