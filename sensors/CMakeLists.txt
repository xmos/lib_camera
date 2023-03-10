# ##############################################################################
# CMake configuration stuff
cmake_minimum_required(VERSION 3.14)
cmake_policy(SET CMP0057 NEW)
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

string(REPLACE "-MD" "-MMD" CMAKE_DEPFILE_FLAGS_C ${CMAKE_DEPFILE_FLAGS_C})

# ##############################################################################
# Target name
set(LIB_NAME lib_imx)

# Source files
file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_XC "*.xc")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE SOURCES_ASM "*.S")

add_library(${LIB_NAME} STATIC)

target_include_directories(${LIB_NAME} PUBLIC .)

target_sources(${LIB_NAME} PRIVATE ${SOURCES_C} ${SOURCES_XC} ${SOURCES_CPP}
                                   $<$<PLATFORM_ID:XCORE_XS3A>:${SOURCES_ASM}>)

target_link_libraries(${LIB_NAME} PUBLIC lib_i2c)

# ##############################################################################
# Add and link the library
add_library(lib_imx_general INTERFACE)

target_link_libraries(lib_imx_general INTERFACE ${LIB_NAME})

add_library(sensors::lib_imx ALIAS lib_imx_general)
