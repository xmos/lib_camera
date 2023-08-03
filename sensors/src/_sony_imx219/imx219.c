#include "imx219.h"

// -----------------------------------------------------
// Low level functions
// -----------------------------------------------------
void imx219_i2c_init(i2c_config_t cfg) {
    i2c_master_init(
        cfg.i2c_ctx_ptr,
        cfg.p_scl, 0, 0,
        cfg.p_sda, 0, 0,
        cfg.speed);
    delay_milliseconds(100);
}

uint16_t imx219_read(i2c_config_t cfg, uint16_t reg) {
    i2c_regop_res_t op_code;

    uint16_t result = read_reg16(
        cfg.i2c_ctx_ptr,
        cfg.device_addr,
        reg,
        &op_code);

    xassert((op_code == I2C_REGOP_SUCCESS) && "Could not read from I2C");
    return result;
}

int imx219_write(i2c_config_t cfg, i2c_settings_t settings){
    i2c_regop_res_t op_code = write_reg8_addr16(
        cfg.i2c_ctx_ptr,
        cfg.device_addr,
        settings.reg_addr,
        settings.reg_val);
    return op_code != I2C_REGOP_SUCCESS ? -1 : 0;
}

int imx219_write_reg_val(i2c_config_t cfg, uint16_t reg, uint8_t val){
    i2c_regop_res_t op_code = write_reg8_addr16(
        cfg.i2c_ctx_ptr,
        cfg.device_addr,
        reg,
        val);
    return op_code != I2C_REGOP_SUCCESS ? -1 : 0;
}

int imx219_write_table(
    i2c_config_t cfg, 
    i2c_settings_t table[],
    size_t table_size) {
    
    const unsigned sleep_adr = 0xFFFF;
    const unsigned sleep_ticks = 200*100;
    int ret = 0;

    for (int i = 0; i < table_size; i++) {
        uint16_t address = table[i].reg_addr;
        uint16_t value = table[i].reg_val;
        
        // pause if we reset the device
        if (address == sleep_adr) {
            delay_ticks(sleep_ticks);
            #if PRINT_I2C_REG
                printf("sleeping...\n");
            #endif
        }

        if (CONTINUOUS_MODE(value, address)){
            if (address & 0x8000) {
                address &= 0x7fff;
            }
            #if PRINT_I2C_REG
                printf("mode=%c , address  = 0x%04x, value = 0x%02x\n", 'c', address, value >> 8);
                printf("mode=%c , address+ = 0x%04x, value = 0x%02x\n", 'c', address+1, value & 0xff);  
            #endif 
            ret  = imx219_write_reg_val(cfg, address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
            ret |= imx219_write_reg_val(cfg, address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4 
        }
        else{
            #if PRINT_I2C_REG
                printf("mode=%c , address = 0x%04x, value = 0x%02x\n", 's', address, value);
            #endif
            ret = imx219_write_reg_val(cfg, address, value);
        }



    }
    return ret;
}


// -----------------------------------------------------
// High level functions
// -----------------------------------------------------
int imx219_initialize(i2c_config_t cfg)
{
    int ret = 0;
    // Send all registers that are common to all modes
    ret = imx219_write_table(cfg, imx219_common_regs, sizeof(imx219_common_regs) / sizeof(imx219_common_regs[0]));
    // Configure two or four Lane mode
    ret = imx219_write_table(cfg, imx219_lanes_regs, sizeof(imx219_lanes_regs) / sizeof(imx219_lanes_regs[0]));
    // set gain
    ret = imx219_set_exposure(cfg, GAIN_DB);
    return ret;
}

int imx219_stream_start(i2c_config_t cfg){
    int ret = 0;
    /* set stream on register */
    ret = imx219_write_table(cfg, start_regs, sizeof(start_regs) / sizeof(start_regs[0]));
    return ret;
}

int imx219_configure(i2c_config_t cfg, regs_config_t reg_cfg)
{
    int ret = 0;
    // Apply default values of current mode
    ret = imx219_write_table(cfg, reg_cfg.regs_frame_size, reg_cfg.regs_frame_size_size);
    // set frame format register
    ret = imx219_write_table(cfg, reg_cfg.regs_pixel_format, reg_cfg.regs_pixel_format_size);
    // set binning
    ret = imx219_write_table(cfg, binning_regs, sizeof(binning_regs) / sizeof(binning_regs[0]));
    return ret;
}

int imx219_stream_stop(i2c_config_t cfg){
    return imx219_write_table(cfg, stop_regs, sizeof(stop_regs) / sizeof(stop_regs[0]));
}

int imx219_set_exposure(
    i2c_config_t cfg,
    uint32_t dBGain)
{
    uint32_t time, again, dgain;
    if (dBGain > GAIN_MAX_DB)
    {
        dBGain = GAIN_MAX_DB;
    }
    if (dBGain < INTEGRATION_TIMES)
    {
        time = gain_integration_times[dBGain];
        again = gain_analogue_gains[0];
        dgain = gain_digital_gains[0];
    }
    else
    {
        time = gain_integration_times[INTEGRATION_TIMES - 1];
        if (dBGain < INTEGRATION_TIMES + ANALOGUE_GAINS)
        {
            again = gain_analogue_gains[dBGain - INTEGRATION_TIMES + 1];
            dgain = gain_digital_gains[0];
        }
        else
        {
            again = gain_analogue_gains[ANALOGUE_GAINS];
            dgain = gain_digital_gains[dBGain - INTEGRATION_TIMES - ANALOGUE_GAINS + 1];
        }
    }
    int ret = imx219_write_reg_val(cfg, 0x0157, again);
    ret |= imx219_write_reg_val(cfg, 0x0158, dgain >> 8);
    ret |= imx219_write_reg_val(cfg, 0x0159, dgain & 0xff);
    ret |= imx219_write_reg_val(cfg, 0x015A, time >> 8);
    ret |= imx219_write_reg_val(cfg, 0x015B, time & 0xff);

    return ret;
}
