// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#ifdef __cplusplus

#include "sensor_base.hpp"
//#include "sensor.h"
#include "sensor_control.h"

namespace sensor {

class IMX219 : public SensorBase {

  private:

    /**
     * @brief Sensor resolution, RAW format and binning mode
     */
    resolution_t frame_res;
    pixel_format_t pix_fmt;
    bool binning_2x2;

    /**
     * @brief X and Y lenghts and offsets
     */
    uint16_t x_len, y_len;
    uint16_t x_offset, y_offset;

    /**
     * @brief Get X and Y lenghts
     */
    void get_x_y_len();

    /**
     * @brief Calculates offsets
     *
     * @param centralise If set, offsets will be calculated to centralise the frame, otherwise will start from (0, 0)
     */
    void get_offsets_and_check_ranges(bool centralize);

    /**
     * @brief Checks that given resolution, binning mode and offsets are within sensor limits
     */
    void check_ranges();

    /**
     * @brief Adjusts offsets so they will be even numbers
     */
    void adjust_offsets();

    /**
     * @brief Get pixel format register table
     *
     * @param format_regs Pointer to I2C line structure to fill
     */
    i2c_table_t get_pxl_fmt_table();

    /**
     * @brief Get resolution register table
     *
     * @param resolution_regs Pointer to I2C line structure to fill
     */
    i2c_table_t get_res_table();

    /**
     * @brief Get exposure gains register table
     *
     * @param dBGain      Exposure gain in dB, can enable different types of camera gain
     */
    i2c_table_t get_exp_gains_table(uint32_t dBGain);

  public:

    /**
     * @brief Construct new `IMX219`
     *
     * @param _conf       I2C master config to use for the sensor control
     * @param _res        Resolution config
     * @param _pix_fmt    RAW format
     * @param _binning    2x2 binning mode
     * @param _centralize If set, offsets will be calculated to centralise the frame, otherwise will start from (0, 0)
     * @note This will initialize I2C interface
     */
    IMX219(i2c_config_t _conf, resolution_t _res, pixel_format_t _pix_fmt, bool _binning, bool _centralize);

    /**
     * @brief Construct new `IMX219`
     *
     * @param _conf       I2C master config to use for the sensor control
     * @param _res        Resolution config
     * @param _pix_fmt    RAW format
     * @param _binning    2x2 binning mode
     * @param _x_offset   X offset
     * @param _y_offset   Y offset
     * @note This will initialize I2C interface
     */
    IMX219(i2c_config_t _conf, resolution_t _res, pixel_format_t _pix_fmt, bool _binning, uint16_t _x_offset, uint16_t _y_offset);

    /**
     * @brief Initialise sensor
     *
     * @returns           0 if succeeded, -1 if failed
     */
    int initialize();

    /**
     * @brief Start data stream
     *
     * @returns           0 if succeeded, -1 if failed
     */
    int stream_start();

    /**
     * @brief Stop data stream
     *
     * @returns           0 if succeeded, -1 if failed
     */
    int stream_stop();

    /**
     * @brief Set sensor exposure
     *
     * @param dBGain      Exposure gain in dB, can enable different types of camera gain
     * @returns           0 if succeeded, -1 if failed
     */
    int set_exposure(uint32_t dBGain);

    /**
     * @brief Set sensor resolution, binning mode, and RAW format
     *
     * @returns           0 if succeeded, -1 if failed
     */
    int configure();

    /**
     * @brief Control thread intry, will initialise and configure sensor inside
     *
     * @param c_control   Control channel
     */
    void control(chanend_t c_control);

}; // IMX219

} // sensor

#endif // __cplusplus
