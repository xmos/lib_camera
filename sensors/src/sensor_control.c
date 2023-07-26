#include "sensor_control.h"

// global I2C context
i2c_master_t i2c_ctx;


// I2C settings
i2c_config_t sony_i2c_cfg = {
    .device_addr = 0x10,
    .speed = 400,
    .p_scl = XS1_PORT_1N,
    .p_sda = XS1_PORT_1O,
    .i2c_ctx_ptr = &i2c_ctx
};


// Register settings
regs_config_t default_sony_reg_config = {
    .regs_frame_size = mode_640_480_regs,
    .regs_frame_size_size = sizeof(mode_640_480_regs) / sizeof(i2c_settings_t),
    .regs_pixel_format = raw8_framefmt_regs,
    .regs_pixel_format_size = sizeof(raw8_framefmt_regs) / sizeof(i2c_settings_t)
};


// Sensor start
void sensor_i2c_start(chanend_t schan[]) {

    // Init I2C
    imx219_i2c_init(sony_i2c_cfg);
    printf("\nI2C initialized...\n");

    // Put a default value
    int r = 0;
    r |= imx219_initialize(sony_i2c_cfg);
    delay_milliseconds(100);
    r |= imx219_configure(sony_i2c_cfg, default_sony_reg_config);
    delay_milliseconds(600);
    r |= imx219_stream_start(sony_i2c_cfg);
    delay_milliseconds(600);
    printf("\nCamera_started and configured...\n");
    assert(r == 0);

    sensor_control(schan);
}


void simple_sensor_control(
    chanend_t schan[])
{
    SELECT_RES(
        CASE_THEN(schan[SENSOR_INIT], sensor_init_handler),
        DEFAULT_THEN(default_handler))
    {
    sensor_init_handler:
        chan_in_word(schan[SENSOR_INIT]);
        printf("Initializing sensor !!!! ...\n");
        SELECT_CONTINUE_RESET;
    default_handler:
        SELECT_CONTINUE_RESET;
    }
}


void sensor_control(
    chanend_t schan[])
{
    regs_config_t tmp_sensor_regs;
    uint32_t tmp_exposure = 40;

    SELECT_RES(
        CASE_THEN(schan[SENSOR_INIT], sensor_init_handler),
        CASE_THEN(schan[SENSOR_CONFIG], sensor_config_handler),
        CASE_THEN(schan[SENSOR_STREAM_START], sensor_stream_start_handler),
        CASE_THEN(schan[SENSOR_STREAM_STOP], sensor_stream_stop_handler),
        CASE_THEN(schan[SENSOR_SET_EXPOSURE], sensor_set_exposure_handler),
        DEFAULT_THEN(default_handler))
    {
    sensor_init_handler:
        chan_in_word(schan[SENSOR_INIT]);
        imx219_initialize(sony_i2c_cfg);
        SELECT_CONTINUE_RESET;
    sensor_config_handler:
        chan_in_buf_byte(schan[SENSOR_CONFIG], (uint8_t*)&tmp_sensor_regs, sizeof(regs_config_t));
        imx219_configure(sony_i2c_cfg, tmp_sensor_regs);
        SELECT_CONTINUE_RESET;
    sensor_stream_start_handler:
        chan_in_word(schan[SENSOR_STREAM_START]);
        imx219_stream_start(sony_i2c_cfg);
        SELECT_CONTINUE_RESET;
    sensor_set_exposure_handler:
        tmp_exposure = chan_in_word(schan[SENSOR_SET_EXPOSURE]);
        printf("--------------- Setting exposure to %ld\n", tmp_exposure);
        imx219_set_exposure(sony_i2c_cfg, tmp_exposure);
        SELECT_CONTINUE_RESET;
    default_handler:
        SELECT_CONTINUE_RESET;
    sensor_stream_stop_handler:
        chan_in_word(schan[SENSOR_STREAM_STOP]);
        printf("--------------- Stopping sensor\n");
        uint32_t r = imx219_stream_stop(sony_i2c_cfg);
        chan_out_word(schan[SENSOR_STREAM_STOP], r);
        return;
    }
}
