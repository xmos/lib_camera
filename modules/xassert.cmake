set(LIB_NAME lib_xassert)
set(LIB_XASSERT_PATH ${CMAKE_CURRENT_LIST_DIR}/xassert/lib_xassert)

add_library(${LIB_NAME} STATIC)

target_sources(${LIB_NAME}
    PRIVATE
        ${LIB_XASSERT_PATH}/src/xassert.xc
)

target_include_directories(${LIB_NAME}
    PUBLIC
        ${LIB_XASSERT_PATH}/api
)

target_compile_options(${LIB_NAME}
    PRIVATE
        -Os -g -Wall -Werror
)
