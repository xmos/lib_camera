# Executable name
set(TARGET i2c_example)  

#**********************
# Targets
#**********************
add_executable(${TARGET})
target_sources(${TARGET}
    PRIVATE
        src/main.xc
)

set(EXAMPLE_APP_LINK_LIBRARIES                 
    fwk_camera::lib_i2c_new
    fwk_camera::mipi
)

target_include_directories(${TARGET} PUBLIC src)
target_compile_options(${TARGET} PRIVATE ${EXAMPLE_APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${EXAMPLE_APP_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${EXAMPLE_APP_LINK_OPTIONS})
