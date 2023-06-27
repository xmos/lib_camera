set(LIB_NAME lib_i2c)
set(LIB_I2C_PATH ${CMAKE_CURRENT_LIST_DIR}/i2c/lib_i2c/)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME}
  PRIVATE
    ${LIB_I2C_PATH}/src/i2c_master_async.xc
    ${LIB_I2C_PATH}/src/i2c_master_ext.xc
    ${LIB_I2C_PATH}/src/i2c_master_single_port.xc
    ${LIB_I2C_PATH}/src/i2c_master.xc
    ${LIB_I2C_PATH}/src/i2c_slave.xc
)

target_include_directories(${LIB_NAME}
  PUBLIC
    ${LIB_I2C_PATH}/api
)

target_compile_options(${LIB_NAME}
  PRIVATE
    -Os -g -Wall -Werror
)

target_link_libraries(${LIB_NAME}
  lib_xassert
)

add_library(i2c::lib_i2c ALIAS ${LIB_NAME})
