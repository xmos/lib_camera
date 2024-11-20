set(LIB_NAME lib_camera)
set(LIB_VERSION 2.0.0)
set(LIB_DEPENDENT_MODULES i2c "lib_logging(3.2.0)")
set(LIB_INCLUDES api src src/sensors)
set(LIB_COMPILER_FLAGS -Os -Wall -g -fxscope -mcmodel=large -Werror)

# I2C
set(XMOS_DEP_DIR_i2c ${XMOS_SANDBOX_DIR}/fwk_io/modules)
if(NOT EXISTS ${XMOS_SANDBOX_DIR}/fwk_io)
    include(FetchContent)
    FetchContent_Declare(
        fwk_io
        GIT_REPOSITORY git@github.com:xmos/fwk_io
        GIT_TAG feature/xcommon_cmake
        SOURCE_DIR ${XMOS_SANDBOX_DIR}/fwk_io
    )
    FetchContent_Populate(fwk_io)
endif()

XMOS_REGISTER_MODULE()
