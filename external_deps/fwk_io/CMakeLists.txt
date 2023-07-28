set(LIB_PATH 
    ${EXT_DEPS_DIR}/fwk_io/modules/i2C
)

###############################
# Use file(GLOB_RECURSE ...) to get the list of files in the target folder
file(GLOB_RECURSE TARGET_FILES ${LIB_PATH}/*)

# Loop through the list of files and print their names using message(...)
foreach(file ${TARGET_FILES})
    message(STATUS "File: ${file}")
endforeach()
###############################

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
