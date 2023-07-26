# set xscope config file path
set(CONFIG_XSCOPE_PATH  ${CMAKE_CURRENT_SOURCE_DIR})

# set compiler/linker flags and liked libraries to be used by all examples
set(EXAMPLE_APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -fxscope
    -mcmodel=large
    -target=${XCORE_TARGET}
    ${CONFIG_XSCOPE_PATH}/config.xscope
)

set(EXAMPLE_APP_LINK_OPTIONS 
    "-report" 
    "-target=${XCORE_TARGET}"
    "${CONFIG_XSCOPE_PATH}/config.xscope"
)

set(EXAMPLE_APP_LINK_LIBRARIES                 
    fwk_camera::mipi
    fwk_camera::sensors
    fwk_camera::camera
    fwk_camera::utils
    fwk_camera::xscope_fileio
    fwk_camera::lib_i2c_new
)

# add the examples
add_subdirectory(take_picture_raw)
add_subdirectory(take_picture_local)

if(${CONFIG_MIPI_FORMAT} STREQUAL ${CONFIG_RAW8})
    # we only support RAW8 images in this example for now
    add_subdirectory(take_picture_downsample)
endif()
