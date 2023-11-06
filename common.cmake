if(NOT DEFINED FWK_CAMERA_ROOT_DIR)
    message(FATAL_ERROR "FWK_CAMERA_ROOT_DIR must be defined")
endif()

set(XMOS_DEPS_ROOT_DIR ${FWK_CAMERA_ROOT_DIR}/../)
set(XMOS_DEP_DIR_mipi ${FWK_CAMERA_ROOT_DIR}/modules)
set(XMOS_DEP_DIR_sensors ${FWK_CAMERA_ROOT_DIR})
set(XMOS_DEP_DIR_camera ${FWK_CAMERA_ROOT_DIR})
set(XMOS_DEP_DIR_utils ${FWK_CAMERA_ROOT_DIR})
set(XMOS_DEP_DIR_i2c ${XMOS_DEPS_ROOT_DIR}/fwk_io/modules)
set(CONFIG_RAW8 0x2A)
set(CONFIG_RAW10 0x2B)
set(CONFIG_MIPI_FORMAT ${CONFIG_RAW8})


if(NOT EXISTS ${XMOS_DEPS_ROOT_DIR}/fwk_io)
    include(FetchContent)
    FetchContent_Declare(
        fwk_io
        GIT_REPOSITORY git@github.com:danielpieczko/fwk_io
        GIT_TAG xcommon_cmake
        SOURCE_DIR ${XMOS_DEPS_ROOT_DIR}/fwk_io
    )
    FetchContent_Populate(fwk_io)
endif()
