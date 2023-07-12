## Target name
set( LIB_NAME lib_mipi )

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME}
  PRIVATE
    src/MipiPacketRx.S
    src/MipiPacketRx.xc
)

target_include_directories(${LIB_NAME}
  PUBLIC
    api
)

target_link_libraries(
  ${LIB_NAME} PUBLIC 
  fwk_camera::camera
  fwk_camera::utils
  fwk_camera::xscope_fileio
)

target_compile_options(${LIB_NAME}
  PRIVATE
    -Os -g -Wall -Werror
)

add_library(fwk_camera::mipi ALIAS ${LIB_NAME})
