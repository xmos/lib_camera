# ##############################################################################
# Sources and definitions
# ##############################################################################
set(TARGET test_camera)

file(GLOB_RECURSE APP_SOURCES ${CMAKE_CURRENT_LIST_DIR}/src/*.*c)

set(APP_COMMON_LINK_LIBRARIES 
        Unity
        camera::lib_camera 
)

# ##############################################################################
# Flags
# ##############################################################################
set(APP_LINK_OPTIONS 
    "-report" 
    "-target=${XCORE_TARGET}"
    -fcmdline-buffer-bytes=1024
    ${CMAKE_CURRENT_LIST_DIR}/config.xscope
)

set(APP_COMPILER_FLAGS
    -Os
    -g
    -report
    -Wall
    -Werror
    -fxscope
    -mcmodel=large
    ${CMAKE_CURRENT_LIST_DIR}/config.xscope
)

set(APP_INCLUDES api)

# ##############################################################################
# Create executable
# ##############################################################################
add_executable(${TARGET} )
target_sources(${TARGET} PUBLIC ${APP_SOURCES})
target_include_directories(${TARGET} PUBLIC ${APP_INCLUDES})
target_compile_options(${TARGET} PUBLIC -g -Os -Wall -Wno-xcore-fptrgroup)
target_link_libraries(${TARGET} PUBLIC ${APP_COMMON_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${APP_LINK_OPTIONS})

# enable testing functionality
add_test(NAME ${TARGET} COMMAND ${TARGET})
