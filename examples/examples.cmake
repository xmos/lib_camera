
# edit subfolder, put your examples here
set(EXAMPLES 
        blink_example
    )

# add all the examples that you need
if(${CMAKE_SYSTEM_NAME} STREQUAL XCORE_XS3A)
    foreach(EXAMPLE ${EXAMPLES})
        include(${CMAKE_CURRENT_LIST_DIR}/${EXAMPLE}/${EXAMPLE}.cmake)
    endforeach(EXAMPLE)
else()
    # Determine OS, set up install dir
    if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
        set(HOST_INSTALL_DIR "$ENV{USERPROFILE}\\.xmos\\bin")
    else()
        set(HOST_INSTALL_DIR "/opt/xmos/bin")
    endif()

    add_subdirectory(modules/xscope_fileio/xscope_fileio/host)
    add_subdirectory(../../)
    install(TARGETS xscope_host_endpoint DESTINATION ${HOST_INSTALL_DIR})
endif()


