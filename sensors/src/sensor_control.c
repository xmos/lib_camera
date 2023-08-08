// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include "sensor_control.h"

i2c_master_t i2c_ctx;
i2c_config_t sony_i2c_cfg;
#if     (CONFIG_MODE == MODE_VGA_640x480)
regs_config_t default_sony_reg_config = DEFAULT_REG_CONF;
#elif   (CONFIG_MODE == MODE_1280x960)
regs_config_t default_sony_reg_config = ALT_REG_CONF;
#else
# error "Given CONFIG_MODE is not currently suported"
#endif

// Sensor start
void sensor_i2c_init() {

    // I2C settings
    sony_i2c_cfg.device_addr = 0x10;
    sony_i2c_cfg.speed = 400;
    sony_i2c_cfg.p_scl = XS1_PORT_1N;
    sony_i2c_cfg.p_sda = XS1_PORT_1O;
    sony_i2c_cfg.i2c_ctx_ptr = &i2c_ctx;

    // Init I2C
    imx219_i2c_init(sony_i2c_cfg);
    puts("\nI2C initialized...");

    // Init the I2C sensor first configuration
    int r = 0;
    r |= sensor_initialize(sony_i2c_cfg); 
    delay_milliseconds(100);
    r |= sensor_configure(sony_i2c_cfg, default_sony_reg_config);
    delay_milliseconds(600);
    r |= sensor_stream_start(sony_i2c_cfg);
    delay_milliseconds(600);
    xassert((r == 0) && "Could not initialise camera");
    puts("\nCamera_started and configured...");
}


void sensor_control(chanend_t c_control) {
    // store the response
    uint32_t encoded_response;
    camera_control_t cmd;
    uint8_t arg;
    regs_config_t cfg;

    // sensor control logic
    while(1){
        encoded_response = chan_in_word(c_control);
        chan_out_word(c_control, 0);
        cmd = (camera_control_t) DECODE_CMD(encoded_response);

        #if ENABLE_PRINT_SENSOR_CONTROL
            printf("--------------- Received command %d\n", cmd);
        #endif
        
        switch (cmd)
        {
        case SENSOR_INIT:
            sensor_initialize(sony_i2c_cfg);
            break;
        case SENSOR_CONFIG:
            //TODO reimplement when dynamic configuration is supported
            sensor_configure(sony_i2c_cfg, cfg);
            break;
        case SENSOR_STREAM_START:
            sensor_stream_start(sony_i2c_cfg);
            break;
        case SENSOR_STREAM_STOP:
            sensor_stream_stop(sony_i2c_cfg);            
            break;
        case SENSOR_SET_EXPOSURE:
            arg = DECODE_ARG(encoded_response);
            sensor_set_exposure(sony_i2c_cfg, arg);
            break;
        default:
            break;
        }
    }
}
