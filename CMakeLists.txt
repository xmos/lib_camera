cmake_minimum_required(VERSION 3.21)

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(fwk_camera)

## Enable languages for project
enable_language(CXX C ASM)

if(NOT ${CMAKE_SYSTEM_PROCESSOR} STREQUAL XCORE_XS3A)
    message(FATAL_ERROR "Only XCORE_XS3A build is supported!")
endif()

## Add frameworks
add_subdirectory(modules)

## Add sensors
add_subdirectory(sensors)

## Add cameras
add_subdirectory(camera)

## Add top level project targets
if(PROJECT_IS_TOP_LEVEL)
    include(examples/examples.cmake)
endif()
