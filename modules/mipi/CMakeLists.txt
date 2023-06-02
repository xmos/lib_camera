
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

target_compile_options(${LIB_NAME}
  PRIVATE
    -Os -g -Wall
)

add_library(mipi::lib_mipi ALIAS ${LIB_NAME})
