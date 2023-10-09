include(FetchContent)

# Fetch library
message(STATUS "Fetching: fwk_camera_deps/fwk_io")
FetchContent_Declare(
  fwk_io
  GIT_REPOSITORY https://github.com/xmos/fwk_io
  GIT_TAG        dcf1a87ddd006c328906094ec12493e604bc9b2f # v3.3.0
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io
)
FetchContent_Populate(fwk_io)

# Add library
add_subdirectory(
  ${CMAKE_BINARY_DIR}/fwk_camera_deps/fwk_io/modules/i2c 
  ${CMAKE_SOURCE_DIR}/build_deps
)
