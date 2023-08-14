# ##############################################################################
# CMake configuration stuff
enable_language(C CXX ASM)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ##############################################################################
# Target name
set(LIB_NAME lib_sensors)

add_library(${LIB_NAME} STATIC)

target_compile_definitions(${LIB_NAME}
  PUBLIC
    CONFIG_MIPI_FORMAT=${CONFIG_MIPI_FORMAT}
)

target_include_directories(${LIB_NAME}
  PUBLIC
    api
    src/_sony_imx219
)

target_sources(${LIB_NAME}
  PRIVATE
    src/SensorBase.cpp
    src/sensor_control.cpp
)

target_compile_options(${LIB_NAME}
  PRIVATE
    -Wall
    -Werror 
)

target_link_libraries(${LIB_NAME} PUBLIC lib_i2c)

add_library(fwk_camera::sensors ALIAS ${LIB_NAME})
