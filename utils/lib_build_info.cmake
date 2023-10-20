set(LIB_NAME io_utils)
set(LIB_VERSION 0.2.1)
set(LIB_INCLUDES io_utils)
set(LIB_C_SRCS io_utils/io_utils.c io_utils/packet_rx_simulate.c)
set(LIB_DEPENDENT_MODULES "")

XMOS_REGISTER_MODULE()
