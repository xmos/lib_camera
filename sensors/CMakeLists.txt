cmake_minimum_required(VERSION 3.14)

# https://github.com/xmos/lib_xs3_math/issues/78
cmake_policy(SET CMP0057 NEW)

project(xmos-lib_mipi)

## CMake configuration stuff
enable_language( C CXX ASM )
set( CMAKE_CXX_STANDARD           11  )
set( CMAKE_CXX_STANDARD_REQUIRED  ON  )
set( CMAKE_CXX_EXTENSIONS         OFF )

string(REPLACE "-MD" "-MMD" CMAKE_DEPFILE_FLAGS_C ${CMAKE_DEPFILE_FLAGS_C})



add_library(lib_imx STATIC)
target_sources(lib_imx PRIVATE imx219.xc)

target_link_libraries(lib_imx lib_i2c)

## Add and link the library
add_library(
    lib_imx_general INTERFACE
)

target_link_libraries(
    lib_imx_general INTERFACE 
        lib_imx
)

add_library(sensors::lib_imx ALIAS lib_imx_general)