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
    printf("\nI2C initialized...\n");

    // Init the I2C sensor first configuration
    int r = 0;
    r |= sensor_initialize(sony_i2c_cfg); 
    delay_milliseconds(100);
    r |= sensor_configure(sony_i2c_cfg, default_sony_reg_config);
    delay_milliseconds(600);
    r |= sensor_stream_start(sony_i2c_cfg);
    delay_milliseconds(600);
    xassert((r == 0) && "Could not initialise camera");
    printf("\nCamera_started and configured...\n");
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
        #if ENABLE_PRINT_SENSOR_CONTROL
            printf("--------------- Received command %d\n", response.cmd);
        #endif

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
    streaming_chanend_t c_control)
{
    s_chan_out_word(c_control, (uint32_t)response.cmd);
    s_chan_out_word(c_control, (uint32_t)response.arg);
}

sensor_cmd_t sensor_ctrl_chan_in_cmd(
    streaming_chanend_t c_control)
{
    sensor_cmd_t response;
    response.cmd = s_chan_in_word(c_control);
    response.arg = s_chan_in_word(c_control);
    return response;
}

void sensor_ctrl_chan_out_cfg_register(
    regs_config_t reg_cfg,
    streaming_chanend_t c_control)
{
    s_chan_out_word(c_control, (uint32_t)reg_cfg.regs_frame_size);
    s_chan_out_word(c_control, (uint32_t)reg_cfg.regs_frame_size_size);
    s_chan_out_word(c_control, (uint32_t)reg_cfg.regs_pixel_format);
    s_chan_out_word(c_control, (uint32_t)reg_cfg.regs_pixel_format_size);
}

regs_config_t sensor_ctrl_chan_in_cfg_register(
    streaming_chanend_t c_control)
{
    regs_config_t reg_cfg;
    reg_cfg.regs_frame_size = (i2c_settings_t*)s_chan_in_word(c_control);
    reg_cfg.regs_frame_size_size = (size_t)s_chan_in_word(c_control);
    reg_cfg.regs_pixel_format = (i2c_settings_t*)s_chan_in_word(c_control);
    reg_cfg.regs_pixel_format_size = (size_t)s_chan_in_word(c_control);
    return reg_cfg;
}
