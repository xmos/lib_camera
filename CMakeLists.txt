cmake_minimum_required(VERSION 3.21)

## Set the target 
set(XCORE_TARGET "XCORE-AI-EXPLORER")

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(fwk_camera)

## Enable languages for project
enable_language(CXX C ASM)

## Fixes unnecessary recompilation of .xc files in Windows builds.
string(REPLACE "-MD" "-MMD" CMAKE_DEPFILE_FLAGS_C ${CMAKE_DEPFILE_FLAGS_C})

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL XCORE_XS3A)

set(CONFIG_RAW8 0x2A)
set(CONFIG_RAW10 0x2B)
set(CONFIG_MIPI_FORMAT ${CONFIG_RAW8})

## Add frameworks
add_subdirectory(modules)
add_subdirectory(external_deps)

## Add sensors
add_subdirectory(sensors)

## Add cameras
add_subdirectory(camera)

if(PROJECT_IS_TOP_LEVEL)
## utils for examples and testing
add_subdirectory(utils)

## Add tests
add_subdirectory(tests)

## Add examples
add_subdirectory(examples)

endif() # top level

endif() # xs3a
