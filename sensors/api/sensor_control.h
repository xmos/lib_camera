#pragma once 

#include <stdio.h>
#include <stdint.h>
// xcore
#include <xcore/select.h>
#include <xcore/channel.h> // includes streaming channel and channend
#include <xccompat.h> // important because streaming channel xc/c compatibility

#include "sensor.h"


#if CONFIG_IMX219_SUPPORT
    #include "imx219.h"
    #define sensor_initialize(cfg)          imx219_initialize(cfg)
    #define sensor_stream_start(cfg)        imx219_stream_start(cfg)
    #define sensor_configure(cfg, rg)       imx219_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     imx219_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         imx219_stream_stop(cfg)

#elif CONFIG_OTHER_SENSOR_SUPPORT           // Demo of how another sensor can be added
    #include "other_sensor.h"
    #define sensor_initialize(cfg)          other_sensor_initialize(cfg)
    #define sensor_stream_start(cfg)        other_sensor_stream_start(cfg)
    #define sensor_configure(cfg, rg)       other_sensor_configure(cfg, rg)
    #define sensor_set_exposure(cfg,ex)     other_sensor_set_exposure(cfg,ex)
    #define sensor_stream_stop(cfg)         other_sensor_stream_stop(cfg)
#endif

// Main control functions
void sensor_i2c_init();
void sensor_control(chanend_t c_control);

// Aux functions
void sensor_ctrl_chan_out_cmd(sensor_cmd_t response, chanend_t c_control);
sensor_cmd_t sensor_ctrl_chan_in_cmd(chanend_t c_control);

void sensor_ctrl_chan_out_cfg_register(regs_config_t reg_cfg, chanend_t c_control);
regs_config_t sensor_ctrl_chan_in_cfg_register(chanend_t c_control);
