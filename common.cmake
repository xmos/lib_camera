if(NOT DEFINED FWK_CAMERA_ROOT_DIR)
    message(FATAL_ERROR "FWK_CAMERA_ROOT_DIR must be defined")
endif()


set(repo_name fwk_camera)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${FWK_CAMERA_ROOT_DIR}/../)
set(XMOS_DEP_DIR_${repo_name} ${FWK_CAMERA_ROOT_DIR})
set(XMOS_DEP_DIR_i2c ${XMOS_SANDBOX_DIR}/fwk_io/modules)
set(xscope_fileio "xscope_fileio(develop)")
set(Unity "danielpieczko/lib_unity")
set(lib_camera ${repo_name})

if(NOT EXISTS ${XMOS_SANDBOX_DIR}/fwk_io)
    include(FetchContent)
    FetchContent_Declare(
        fwk_io
        GIT_REPOSITORY git@github.com:danielpieczko/fwk_io
        GIT_TAG xcommon_cmake
        SOURCE_DIR ${XMOS_SANDBOX_DIR}/fwk_io
    )
    FetchContent_Populate(fwk_io)
endif()

# RAW format configuration
set(CONFIG_RAW8 0x2A)
set(CONFIG_RAW10 0x2B)
set(CONFIG_MIPI_FORMAT ${CONFIG_RAW8})
