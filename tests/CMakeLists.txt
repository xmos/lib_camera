# set xscope config file path for tests
set(CONFIG_XSCOPE_PATH_TEST  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(unit_tests)
add_subdirectory(hardware_tests)
include(unity.cmake) # custom cmake file
