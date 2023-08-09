# Fetch library
include(FetchContent)
FetchContent_Declare(
  fwk_io
  GIT_REPOSITORY https://github.com/xmos/fwk_io
  GIT_TAG        develop
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io
)
FetchContent_Populate(fwk_io)

# Add library
add_subdirectory(
  ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io/modules/i2c 
  ${CMAKE_SOURCE_DIR}/build_deps
)
