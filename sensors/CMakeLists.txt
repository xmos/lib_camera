# ##############################################################################
# CMake configuration stuff
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ##############################################################################
# Target name
set(LIB_NAME lib_imx)

# Source files
file(GLOB_RECURSE SOURCES_C "*.c")
file(GLOB_RECURSE SOURCES_XC "*.xc")
file(GLOB_RECURSE SOURCES_CPP "*.cpp")
file(GLOB_RECURSE SOURCES_ASM "*.S")

add_library(${LIB_NAME} STATIC)

target_compile_definitions(${LIB_NAME}
    PUBLIC
        CONFIG_MIPI_FORMAT=${CONFIG_MIPI_FORMAT}
)

target_include_directories(${LIB_NAME}
    PUBLIC
        api
        src
        _galaxycore_gc2145
        _sony_imx219
)

target_sources(${LIB_NAME}
    PRIVATE
        ${SOURCES_C}
        ${SOURCES_XC}
        ${SOURCES_CPP}
        ${SOURCES_ASM}
)

target_compile_options(${LIB_NAME}
    PUBLIC
        -Wall -Werror
)

target_link_libraries(${LIB_NAME} PUBLIC lib_i2c)

add_library(fwk_camera::sensors ALIAS ${LIB_NAME})
