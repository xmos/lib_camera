if(NOT DEFINED FWK_CAMERA_ROOT_DIR)
    message(FATAL_ERROR "FWK_CAMERA_ROOT_DIR must be defined")
endif()


set(repo_name fwk_camera)

# Sandbox configuration (#TODO remove patch after repo rename)
set(XMOS_SANDBOX_DIR ${FWK_CAMERA_ROOT_DIR}/../)
set(XMOS_DEP_DIR_${repo_name} ${FWK_CAMERA_ROOT_DIR})

# Library
set(lib_camera ${repo_name})

# External deps
set(Unity "lib_unity(2.5.2)")
set(xscope_fileio "xscope_fileio(develop)")
