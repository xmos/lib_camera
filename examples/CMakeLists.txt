# set xscope config file path
set(CONFIG_XSCOPE_PATH  ${CMAKE_CURRENT_SOURCE_DIR})

# set compiler/linker flags and liked libraries to be used by all examples
set(EXAMPLE_APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
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
    mipi::lib_mipi
    i2c::lib_i2c
    sensors::lib_imx
    camera::lib_camera
    fwk_camera::utils
    fwk_camera::xscope_fileio
)

# add the examples
add_subdirectory(take_picture_raw)
add_subdirectory(take_picture_downsample)
