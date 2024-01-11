set(FWK_CAMERA_ROOT_DIR ${CMAKE_SOURCE_DIR}/..) #TODO delete after rename
include(${FWK_CAMERA_ROOT_DIR}/common.cmake)

set(LIB_NAME ${lib_camera})
set(LIB_VERSION 0.2.1)
set(LIB_DEPENDENT_MODULES i2c )
set(LIB_INCLUDES api src/sensors/_sony_imx219)
set(LIB_COMPILER_FLAGS -Os -Wall -Werror -g -fxscope -mcmodel=large)

XMOS_REGISTER_MODULE()
