
set(XSCOPE_FILEIO_PATH ${CMAKE_CURRENT_LIST_DIR}/xscope_fileio)

set(LIB_NAME fwk_camera_xscope_fileio) 
add_library(${LIB_NAME}  INTERFACE)
        
target_sources(${LIB_NAME}
    INTERFACE
        ${XSCOPE_FILEIO_PATH}/xscope_fileio/src/xscope_io_device.c
)

target_include_directories(${LIB_NAME}
    INTERFACE
        ${XSCOPE_FILEIO_PATH}/xscope_fileio
        ${XSCOPE_FILEIO_PATH}/xscope_fileio/api
)

add_library(fwk_camera::xscope_fileio ALIAS ${LIB_NAME})
