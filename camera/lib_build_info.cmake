set(LIB_NAME camera)
set(LIB_VERSION 0.2.1)
set(LIB_INCLUDES api)
set(LIB_DEPENDENT_MODULES mipi sensors)
set(LIB_COMPILER_FLAGS -Os -Wall -Werror -g -fxscope -mcmodel=large)

XMOS_REGISTER_MODULE()
