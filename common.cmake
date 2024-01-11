if(NOT DEFINED FWK_CAMERA_ROOT_DIR)
    message(FATAL_ERROR "FWK_CAMERA_ROOT_DIR must be defined")
endif()


set(repo_name fwk_camera)

# Sandbox configuration
set(XMOS_SANDBOX_DIR ${FWK_CAMERA_ROOT_DIR}/../)
set(XMOS_DEP_DIR_${repo_name} ${FWK_CAMERA_ROOT_DIR})
set(xscope_fileio "xscope_fileio(develop)")
set(Unity "lib_unity(2.5.2)")
set(lib_camera ${repo_name})

# RAW format configuration
set(CONFIG_RAW8 0x2A)
set(CONFIG_RAW10 0x2B)
set(CONFIG_MIPI_FORMAT ${CONFIG_RAW8})
