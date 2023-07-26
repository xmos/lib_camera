#include "cam_i2c.h"




void camera_i2c_master() {
    // create context
    i2c_master_t i2c_ctx;

    // I2C settings
    i2c_config_t sony_i2c_cfg = {
        .device_addr = 0x10,
        .speed = 400,
        .p_scl = XS1_PORT_1N,
        .p_sda = XS1_PORT_1O
    };
    sony_i2c_cfg.i2c_ctx_ptr = &i2c_ctx;

    // Register settings
    regs_config_t sony_reg_config = {
        .regs_frame_size = mode_640_480_regs,
        .regs_pixel_format = raw8_framefmt_regs,
    };


    // Init I2C
    imx219_i2c_init(sony_i2c_cfg);


    // read
    uint16_t reg = 0x0164;  
    uint16_t result =  imx219_read(sony_i2c_cfg, reg);
    printf("Read from register 0x%04x: 0x%04x\n", reg, result);
    int r = 0;
    r |= imx219_initialize(sony_i2c_cfg);
    delay_milliseconds(100);
    r |= imx219_configure(sony_i2c_cfg, sony_reg_config);
    delay_milliseconds(600);
    r |= imx219_stream_start(sony_i2c_cfg);

    printf("\nCamera_started and configured...\n");
    printf("r = %d\n", r);

    result =  imx219_read(sony_i2c_cfg, reg);
    printf("Read from register 0x%04x: 0x%04x\n", reg, result);

}
