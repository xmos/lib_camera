#**********************
# Gather Sources
#**********************

# <--- Set the executable
set(TARGET example_take_picture_raw)  

#**********************
# Flags
#**********************
set(APP_COMPILER_FLAGS
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

set(APP_LINK_OPTIONS 
    "-report" 
    "-target=${XCORE_TARGET}"
    "${CONFIG_XSCOPE_PATH}/config.xscope"
)

# <--- Link libraries
set(APP_COMMON_LINK_LIBRARIES                 
    mipi::lib_mipi
    i2c::lib_i2c
    sensors::lib_imx
    camera::lib_camera
    fwk_camera::utils
    fwk_camera::xscope_fileio
)

#**********************
# Tile Targets
#**********************
add_executable(${TARGET})
target_sources(${TARGET} 
    PRIVATE
        src/app_raw.c
        src/main.xc
)

target_include_directories(${TARGET} PUBLIC src)
target_compile_options(${TARGET} PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${APP_LINK_OPTIONS})
