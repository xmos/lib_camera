
include(xscope_fileio.cmake)

set(LIB_NAME fwk_camera_utils)
add_library(${LIB_NAME} INTERFACE)

target_sources(${LIB_NAME} 
    INTERFACE 
    io_utils/io_utils.c
    io_utils/packet_rx_simulate.c
)

target_include_directories(${LIB_NAME} INTERFACE io_utils)

target_link_libraries(${LIB_NAME} INTERFACE 
  fwk_camera::mipi 
  fwk_camera::camera)

add_library(fwk_camera::utils ALIAS ${LIB_NAME})
