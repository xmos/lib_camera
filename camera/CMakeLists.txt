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
set(LIB_NAME lib_camera)
set(LIB_NAME_ALIAS lib_camera_general)

# Source files
file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_XC "*.xc")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE SOURCES_ASM "*.S")

add_library(${LIB_NAME} STATIC)

target_compile_options(${LIB_NAME}
  PUBLIC
    -Os
    -g
    -fxscope
    -mcmodel=large )

target_include_directories(${LIB_NAME} PUBLIC . api)

target_sources(${LIB_NAME} PRIVATE ${SOURCES_C} ${SOURCES_XC} ${SOURCES_CPP}
                                   $<$<PLATFORM_ID:XCORE_XS3A>:${SOURCES_ASM}>)


# ##############################################################################
# Add and link the library
add_library(${LIB_NAME_ALIAS} INTERFACE)

target_link_libraries(${LIB_NAME} PUBLIC mipi::lib_mipi sensors::lib_imx)

target_link_libraries(${LIB_NAME_ALIAS} INTERFACE ${LIB_NAME})

add_library(camera::lib_camera ALIAS ${LIB_NAME_ALIAS})
