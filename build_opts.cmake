if(NOT DEFINED ROOT_DIR)
    message(FATAL_ERROR "ROOT_DIR must be defined")
endif()

# Sandbox configuration (#TODO remove patch after repo rename)
set(XMOS_SANDBOX_DIR ${ROOT_DIR}/../)
set(XMOS_DEP_DIR_${repo_name} ${ROOT_DIR})

# External deps
set(Unity "lib_unity(2.5.2)")
set(xscope_fileio "xscope_fileio(develop)")
