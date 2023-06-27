# ##############################################################################
# Target name
set(LIB_NAME Unity)
set(LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/Unity)

add_library(${LIB_NAME} STATIC)

target_include_directories(${LIB_NAME}
  PUBLIC 
    ${LIB_PATH}/src
    ${LIB_PATH}/extras/fixture/src
    ${LIB_PATH}/extras/memory/src
)

target_sources(${LIB_NAME}
  PUBLIC
    ${LIB_PATH}/src/unity.c
    ${LIB_PATH}/extras/fixture/src/unity_fixture.c
    ${LIB_PATH}/extras/memory/src/unity_memory.c
)

# Suppressing weird warnings coming from unity
target_compile_options(${LIB_NAME}
  PUBLIC
    -Wno-xcore-fptrgroup
)

add_library(Unity::framework ALIAS ${LIB_NAME})
