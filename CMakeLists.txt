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

## Add frameworks
add_subdirectory(modules)

## Add sensors
add_subdirectory(sensors)

## Add cameras
add_subdirectory(camera)

## Add tests
add_subdirectory(tests)

## Add examples
add_subdirectory(examples)

endif() # xs3a
