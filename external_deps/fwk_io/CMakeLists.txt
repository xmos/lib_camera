set(LIB_PATH 
    ${EXT_DEPS_DIR}/fwk_io/modules/i2C
)

set(LIB_NAME lib_i2c) 

add_library(${LIB_NAME}  STATIC)
    
target_include_directories(${LIB_NAME}
    PUBLIC
    ${LIB_PATH}
    ${LIB_PATH}/api
    ${LIB_PATH}/src
)

target_sources(${LIB_NAME}
    PRIVATE
    ${LIB_PATH}/src/i2c_master.c
    ${LIB_PATH}/src/i2c_slave.c
)

add_library(
    fwk_camera::i2c 
    ALIAS 
    lib_i2c)
