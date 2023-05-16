# edit here --------------------------------
set(PROJECT_NAME first_project)

# do not edit ----------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)

## Disable in-source build.
if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "In-source build is not allowed! Please specify a build folder.\n\tex:cmake -B build")
endif()

## Project declaration
project(PROJECT_NAME)

## Enable languages for project
enable_language(CXX C ASM)

## Project options
option(XCORE_VOICE_TESTS "Enable XCORE-VOICE tests"  OFF)

## Import some helpful macros
include(xmos_cmake_toolchain/xmos_macros.cmake)

## Setup a root path
set(first_project_PATH ${PROJECT_SOURCE_DIR} CACHE STRING "Root folder of sln_voice in this cmake project tree")


## Add frameworks
if(IS_DIRECTORY modules)
    add_subdirectory(modules)
    message("Adding modules/")
endif()

## Add sensors
add_subdirectory(sensors)
message("Adding sensors/")

## Add cameras
add_subdirectory(camera)
message("Adding camera/")



## Add top level project targets
if(PROJECT_IS_TOP_LEVEL)
    include(examples/examples.cmake)
endif()

