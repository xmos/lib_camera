# adding submodules here
add_subdirectory(core)
add_subdirectory(mipi)
# lib_i2c and lib_xassert do not have CMakeLists.txt
# so using wrappers here
include(xassert.cmake)
include(i2c.cmake)
