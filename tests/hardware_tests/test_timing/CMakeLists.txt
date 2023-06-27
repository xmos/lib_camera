# ##############################################################################
set(TARGET test_timing)  

set(SOURCES
    src/app.xc
    src/main.xc
    src/MipiGatherTiming.S
)

set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -target=${XCORE_TARGET}
    ${CONFIG_XSCOPE_PATH_TEST}/config.xscope
)

set(APP_LINK_OPTIONS 
    "-report" 
    "-target=${XCORE_TARGET}"
    "${CONFIG_XSCOPE_PATH_TEST}/config.xscope"
)

set(APP_COMMON_LINK_LIBRARIES                    
    mipi::lib_mipi
    i2c::lib_i2c
    sensors::lib_imx 
    camera::lib_camera
)

# ##############################################################################
add_executable(${TARGET})
target_sources(${TARGET} PUBLIC ${SOURCES})
target_compile_options(${TARGET} PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${APP_LINK_OPTIONS})
