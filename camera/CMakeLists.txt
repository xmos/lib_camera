# ##############################################################################
# CMake configuration stuff
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ##############################################################################
# Target name
set(LIB_NAME lib_camera)

# Source files
file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_XC "*.xc")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE SOURCES_ASM "*.S")

add_library(${LIB_NAME} STATIC)

target_include_directories(${LIB_NAME} PUBLIC . api)

target_sources(${LIB_NAME}
    PRIVATE
        ${SOURCES_C}
        ${SOURCES_XC}
        ${SOURCES_CPP}
        ${SOURCES_ASM}
)

target_compile_options(${LIB_NAME}
  PUBLIC
    -Os
    -g
    -fxscope
    -mcmodel=large )

target_link_libraries(${LIB_NAME} PUBLIC mipi::lib_mipi sensors::lib_imx)
add_library(camera::lib_camera ALIAS ${LIB_NAME})
