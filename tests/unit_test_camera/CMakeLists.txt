# ##############################################################################
# Sources and definitions
# ##############################################################################
set(TARGET test_camera)

file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.*c)

set(APP_COMMON_LINK_LIBRARIES 
        tests::Unity
        camera::lib_camera 
    )

# ##############################################################################
# Flags
# ##############################################################################
set(APP_LINK_OPTIONS -report 
    ${CMAKE_CURRENT_LIST_DIR}/XCORE-AI-EXPLORER.xn
    ${CMAKE_CURRENT_LIST_DIR}/config.xscope
)

set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -fxscope
    -mcmodel=large
    -Wno-xcore-fptrgroup
    ${CMAKE_CURRENT_LIST_DIR}/config.xscope
    ${CMAKE_CURRENT_LIST_DIR}/XCORE-AI-EXPLORER.xn
)

set(APP_COMPILE_DEFINITIONS
    configENABLE_DEBUG_PRINTF=1
    XUD_CORE_CLOCK=600)


# ##############################################################################
# Create executable
# ##############################################################################
add_executable(${TARGET} EXCLUDE_FROM_ALL)
target_sources(${TARGET} PUBLIC ${APP_SOURCES})
target_include_directories(${TARGET} PUBLIC ${APP_INCLUDES})
target_compile_definitions(${TARGET} PRIVATE ${APP_COMPILE_DEFINITIONS})
target_compile_options(${TARGET} PRIVATE ${APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${APP_LINK_OPTIONS})