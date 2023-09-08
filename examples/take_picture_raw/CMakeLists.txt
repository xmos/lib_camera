# Executable name
set(TARGET example_take_picture_raw)  

#**********************
# Targets
#**********************
add_executable(${TARGET})
target_sources(${TARGET} 
  PRIVATE
    src/app_raw.c
    src/main.xc
)

target_include_directories(${TARGET} PUBLIC src)
target_compile_options(${TARGET} PRIVATE ${EXAMPLE_APP_COMPILER_FLAGS})
target_link_libraries(${TARGET} PUBLIC ${EXAMPLE_APP_LINK_LIBRARIES})
target_link_options(${TARGET} PRIVATE ${EXAMPLE_APP_LINK_OPTIONS})
