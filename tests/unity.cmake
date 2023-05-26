# ##############################################################################
# Target name
set(LIB_NAME lib_Unity)
set(LIB_PATH ${CMAKE_CURRENT_LIST_DIR}/Unity/src)

# Source files
file(GLOB_RECURSE SOURCES_C   ${LIB_PATH} *.c)
file(GLOB_RECURSE SOURCES_XC  ${LIB_PATH} *.xc)
file(GLOB_RECURSE SOURCES_CPP ${LIB_PATH} *.cpp)
file(GLOB_RECURSE SOURCES_ASM ${LIB_PATH} *.S)

add_library(${LIB_NAME} STATIC)

target_include_directories(
    ${LIB_NAME} PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/Unity/src/
)

target_sources(${LIB_NAME}
    PRIVATE
        ${SOURCES_C}
        ${SOURCES_XC}
        ${SOURCES_CPP}
        ${SOURCES_ASM}
)

add_library(tests::Unity ALIAS ${LIB_NAME})
