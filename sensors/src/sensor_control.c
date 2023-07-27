#include "sensor_control.h"


i2c_master_t i2c_ctx;
i2c_config_t sony_i2c_cfg;
regs_config_t default_sony_reg_config;

// Sensor start
void sensor_i2c_init() {

    // I2C settings
    sony_i2c_cfg.device_addr = 0x10;
    sony_i2c_cfg.speed = 400;
    sony_i2c_cfg.p_scl = XS1_PORT_1N;
    sony_i2c_cfg.p_sda = XS1_PORT_1O;
    sony_i2c_cfg.i2c_ctx_ptr = &i2c_ctx;

    // Register settings
    default_sony_reg_config.regs_frame_size = mode_640_480_regs;
    default_sony_reg_config.regs_frame_size_size = sizeof(mode_640_480_regs) / sizeof(i2c_settings_t);
    default_sony_reg_config.regs_pixel_format = raw8_framefmt_regs;
    default_sony_reg_config.regs_pixel_format_size = sizeof(raw8_framefmt_regs) / sizeof(i2c_settings_t);

    // Init I2C
    imx219_i2c_init(sony_i2c_cfg);
    printf("\nI2C initialized...\n");

    // Init the I2C sensor first configuration
    int r = 0;
    r |= sensor_initialize(sony_i2c_cfg); 
    delay_milliseconds(100);
    r |= sensor_configure(sony_i2c_cfg, default_sony_reg_config);
    delay_milliseconds(600);
    r |= sensor_stream_start(sony_i2c_cfg);
    delay_milliseconds(600);
    printf("\nCamera_started and configured...\n");
    assert(r == 0);
}

void sensor_control(chanend_t c_control) {
    // store the response
    sensor_cmd_t response;
    regs_config_t cfg;

    // sensor control logic
    SELECT_RES(
        CASE_THEN(c_control, sensor_ctrl_handler),
        DEFAULT_THEN(default_handler))
    {
    sensor_ctrl_handler:

        response = sensor_ctrl_chan_in_cmd(c_control);
        printf("--------------- Received command %d\n", response.cmd);

        switch (response.cmd)
        {
        case SENSOR_INIT:
            sensor_initialize(sony_i2c_cfg);
            break;
        case SENSOR_CONFIG:
            cfg = sensor_ctrl_chan_in_cfg_register(c_control);
            sensor_configure(sony_i2c_cfg, cfg);
            break;
        case SENSOR_STREAM_START:
            sensor_stream_start(sony_i2c_cfg);
            break;
        case SENSOR_STREAM_STOP:
            sensor_stream_stop(sony_i2c_cfg);
            return;
        case SENSOR_SET_EXPOSURE:
            sensor_set_exposure(sony_i2c_cfg, response.arg);
            break;
        default:
            break;
        }

    default_handler:
        continue;
    }
}

// Aux functions
void sensor_ctrl_chan_out_cmd(
    sensor_cmd_t response,
    chanend_t c_control)
{
    chan_out_word(c_control, (uint32_t)response.cmd);
    chan_out_word(c_control, (uint32_t)response.arg);
}

sensor_cmd_t sensor_ctrl_chan_in_cmd(
    chanend_t c_control)
{
    sensor_cmd_t response;
    response.cmd = chan_in_word(c_control);
    response.arg = chan_in_word(c_control);
    return response;
}

void sensor_ctrl_chan_out_cfg_register(
    regs_config_t reg_cfg,
    chanend_t c_control)
{
    chan_out_word(c_control, (uint32_t)reg_cfg.regs_frame_size);
    chan_out_word(c_control, (uint32_t)reg_cfg.regs_frame_size_size);
    chan_out_word(c_control, (uint32_t)reg_cfg.regs_pixel_format);
    chan_out_word(c_control, (uint32_t)reg_cfg.regs_pixel_format_size);
}

regs_config_t sensor_ctrl_chan_in_cfg_register(
    chanend_t c_control)
{
    regs_config_t reg_cfg;
    reg_cfg.regs_frame_size = (i2c_settings_t*)chan_in_word(c_control);
    reg_cfg.regs_frame_size_size = (size_t)chan_in_word(c_control);
    reg_cfg.regs_pixel_format = (i2c_settings_t*)chan_in_word(c_control);
    reg_cfg.regs_pixel_format_size = (size_t)chan_in_word(c_control);
    return reg_cfg;
}
