// Copyright 2023-2025 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

#include <stdint.h>

#include <xcore/assert.h>
#include <xcore/channel.h>

#ifdef __cplusplus

extern "C" {
#include "i2c.h"
}

namespace sensor {

typedef struct
{
  uint16_t reg_addr;
  uint16_t reg_val;
} i2c_line_t;

typedef struct
{
  i2c_line_t * table;
  size_t num_lines;
} i2c_table_t;

typedef struct 
{
  uint8_t  device_addr;
  uint16_t speed;
  port_t p_scl;
  port_t p_sda;
  i2c_master_t * i2c_ctx_ptr;
} i2c_config_t;

#define GET_NUM_LINES(regs_arr) (sizeof(regs_arr) / sizeof(i2c_line_t))
#define GET_TABLE(regs_arr) (i2c_table_t){regs_arr, GET_NUM_LINES(regs_arr)}

/**
 *  @brief Base class for implementing Sensor control from
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
     * @returns           Register value, -1 if failed
     */
    int i2c_read(uint16_t reg);

    /**
     * @brief Write to a single register
     *
     * @param line        I2C line config to write
     * @returns           0 if succeeded, -1 if failed
     */
    int i2c_write_line(i2c_line_t line);

    /**
     * @brief Write to a single register
     *
     * @param reg         Register value to write to
     * @param val         Value to write
     * @returns           0 if succeeded, -1 if failed
     */
    int i2c_write_line(uint16_t reg, uint8_t val);

    /**
     * @brief Write a table of register values
     *
     * @param table       I2C table config to write
     * @returns           0 if succeeded, -1 if failed
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
     */
    virtual int initialize();

    /**
     * @brief Start data stream
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int stream_start();

    /**
     * @brief Stop data stream
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int stream_stop();

    /**
     * @brief Set sensor exposure
     *
     * @param dBGain      Exposure gain in dB, can enable different types of camera gain
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int set_exposure(uint32_t dBGain);

    /**
     * @brief Set sensor resolution, binning mode, and RAW format
     *
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int configure();

    /**
     * @brief Control thread intry, will initialise and configure sensor inside
     *
     * @param c_control   Control channel
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual void control(chanend_t c_control);

    /**
     * @brief Set test pattern
     *
     * @param pattern     Test pattern to set
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int set_test_pattern(uint16_t pattern);

    /**
     * @brief Check if sensor is connected
     * 
     * @returns           0 if succeeded, -1 if failed
     * @note This is a virtual function, and will have to be implemented in the derived class
     */
    virtual int check_sensor_is_connected();

}; // SensorBase

} // sensor

#endif // __cplusplus
