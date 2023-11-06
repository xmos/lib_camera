set(LIB_NAME sensors)
set(LIB_VERSION 0.2.1)
set(LIB_INCLUDES api src/_sony_imx219)
set(LIB_DEPENDENT_MODULES i2c)
set(LIB_COMPILER_FLAGS -Wall -Werror -DCONFIG_MIPI_FORMAT=${CONFIG_MIPI_FORMAT})

XMOS_REGISTER_MODULE()