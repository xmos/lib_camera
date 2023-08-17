// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <xcore/assert.h>
#include <xcore/channel.h>

#include "sensor_settings.h"

#ifdef __cplusplus

namespace sensor {

/**
 *  @brief Base class for implementing Sensor contol from
 */
class SensorBase {

  private:

    /**
     * @brief I2C master config to be used for the sensor control
     */
    i2c_config_t i2c_cfg;

    /**
     * @brief Intialise I2C master interface
     */
    void i2c_init();

  protected:

    /**
     * @brief Read from a 16-bit register
     *
     * @param reg         Register to read from
     */
    uint16_t i2c_read(uint16_t reg);

    /**
     * @brief Write to a single register
     *
     * @param line        I2C line config to write
     */
    int i2c_write_line(i2c_line_t line);

    /**
     * @brief Write to a single register
     *
     * @param reg         Register value to write to
     * @param val         Value to write
     */
    int i2c_write_line(uint16_t reg, uint8_t val);

    /**
     * @brief Write a table of register values
     *
     * @param table       I2C table config to write
     */
    int i2c_write_table(i2c_table_t table);

  public:

    /**
     * @brief Construct new `SensorBase`
     *
     * @param _conf       I2C master config to use for the sensor control
     * @note This will initialize I2C interface
     */
    SensorBase(i2c_config_t _conf);

    /**
     * @brief Initialize sensor
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual int initialize();

    /**
     * @brief Start data stream
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual int stream_start();

    /**
     * @brief Stop data stream
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual int stream_stop();

    /**
     * @brief Set sensor exposure
     *
     * @param dBGain      Exposure gain in dB, can enable different types of camera gain
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual int set_exposure(uint32_t dBGain);

    /**
     * @brief Set sensor resolution, binning mode, and RAW format
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual int configure();

    /**
     * @brief Control thread intry, will initialise and configure sensor inside
     *
     * @param c_control   Control channel
     * @note This is a virtual function, and will have to be implemented in the derived class
     * @note Can't make it a pure virtual method as it adds 18 kB of memory in the current (15.2.1) XTC version
     */
    virtual void control(chanend_t c_control);

}; // SensorBase

} // sensor

#endif // __cplusplus
