# set xscope config file path
set(CONFIG_XSCOPE_PATH  ${CMAKE_CURRENT_SOURCE_DIR})

# add the examples
add_subdirectory(simple_timing)
add_subdirectory(take_picture_raw)
add_subdirectory(take_picture_downsample)
