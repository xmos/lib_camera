include(FetchContent)

# Fetch library: fwk_io
if(EXISTS ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io)
  message(STATUS "Skipped: fwk_camera_deps/fwk_io")
else()
  message(STATUS "Fetching: fwk_camera_deps/fwk_io")
  FetchContent_Declare(
    fwk_io
    GIT_REPOSITORY https://github.com/xmos/fwk_io
    GIT_TAG        cf70def312a43c011e1f6b37cededdc379ca3bf4 # v3.1.0
    GIT_SHALLOW    TRUE
    SOURCE_DIR     ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io
  )
  FetchContent_Populate(fwk_io)
endif()

# Add library: fwk_io
add_subdirectory(
  ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io/modules/i2c 
  ${CMAKE_SOURCE_DIR}/build_deps
)
