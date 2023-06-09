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
    -fxscope
    -mcmodel=large
    -Wno-xcore-fptrgroup
    -target=${XCORE_TARGET}
    ${CONFIG_XSCOPE_PATH}/config.xscope
)


set(APP_COMPILE_DEFINITIONS
    configENABLE_DEBUG_PRINTF=1
    PLATFORM_SUPPORTS_TILE_0=1
    PLATFORM_SUPPORTS_TILE_1=1
    PLATFORM_SUPPORTS_TILE_2=0
    PLATFORM_SUPPORTS_TILE_3=0
    PLATFORM_USES_TILE_0=1
    PLATFORM_USES_TILE_1=1
    XUD_CORE_CLOCK=600
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
target_compile_definitions(${TARGET} PRIVATE ${APP_COMPILE_DEFINITIONS})
target_compile_options(${TARGET} PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${APP_LINK_OPTIONS})
