// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <stdio.h>

#include "sensor_base.hpp"

using namespace sensor;

SensorBase::SensorBase(i2c_config_t _conf) : i2c_cfg(_conf) {
  this->i2c_init();
}

void SensorBase::i2c_init() {
  i2c_master_init(
    this->i2c_cfg.i2c_ctx_ptr,
    this->i2c_cfg.p_scl, 0, 0,
    this->i2c_cfg.p_sda, 0, 0,
    this->i2c_cfg.speed);
  delay_milliseconds(100);
  puts("\nI2C initialized...");
}

uint16_t SensorBase::i2c_read(uint16_t reg) {
  i2c_regop_res_t op_code;

  uint16_t result = read_reg16(
    this->i2c_cfg.i2c_ctx_ptr,
    this->i2c_cfg.device_addr,
    reg,
    &op_code);

  xassert((op_code == I2C_REGOP_SUCCESS) && "Could not read from I2C");
  return result;
}

int SensorBase::i2c_write_line(i2c_line_t line) {
  i2c_regop_res_t op_code = write_reg8_addr16(
    this->i2c_cfg.i2c_ctx_ptr,
    this->i2c_cfg.device_addr,
    line.reg_addr,
    line.reg_val);
  return op_code != I2C_REGOP_SUCCESS ? -1 : 0;
}

int SensorBase::i2c_write_line(uint16_t reg, uint8_t val) {
  i2c_regop_res_t op_code = write_reg8_addr16(
    this->i2c_cfg.i2c_ctx_ptr,
    this->i2c_cfg.device_addr,
    reg,
    val);
  return op_code != I2C_REGOP_SUCCESS ? -1 : 0;
}

int SensorBase::i2c_write_table(i2c_table_t table) {
  const unsigned sleep_adr = 0xFFFF;
  const unsigned sleep_ticks = 200 * 100;
  int ret = 0;

  for (size_t i = 0; i < table.num_lines; i++) {
    uint16_t address = table.table[i].reg_addr;
    uint16_t value = table.table[i].reg_val;
    
    // pause if we reset the device
    if (address == sleep_adr) {
      delay_ticks(sleep_ticks);
      #if PRINT_I2C_REG
        printf("sleeping...\n");
      #endif
    }

    // if continuous mode
    if ((value & 0xFF00) || (address & 0x8000)) {
      if (address & 0x8000) {
        address &= 0x7fff;
      }
      #if PRINT_I2C_REG
        printf("mode=%c , address  = 0x%04x, value = 0x%02x\n", 'c', address, value >> 8);
        printf("mode=%c , address+ = 0x%04x, value = 0x%02x\n", 'c', address+1, value & 0xff);  
      #endif 
      ret |= this->i2c_write_line(address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
      ret |= this->i2c_write_line(address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4 
    }
    else {
      #if PRINT_I2C_REG
        printf("mode=%c , address = 0x%04x, value = 0x%02x\n", 's', address, value);
      #endif
      ret |= this->i2c_write_line(address, (uint8_t)value);
    }
  }
  return ret != I2C_REGOP_SUCCESS ? -1 : 0;
}

int SensorBase::initialize() {
  xassert(0 && "Sensor Exception: Make sure your initialize() method is implemented and called from the derived class");
  return -1;
}

int SensorBase::stream_start() {
  xassert(0 && "Sensor Exception: Make sure your stream_start() method is implemented and called from the derived class");
  return -1;
}

int SensorBase::stream_stop() {
  xassert(0 && "Sensor Exception: Make sure your stream_stop() method is implemented and called from the derived class");
  return -1;
}

int SensorBase::set_exposure(uint32_t dBGain) {
  (void)dBGain;
  xassert(0 && "Sensor Exception: Make sure your set_exposure() method is implemented and called from the derived class");
  return -1;
}

int SensorBase::configure() {
  xassert(0 && "Sensor Exception: Make sure your configure() method is implemented and called from the derived class");
  return -1;
}

void SensorBase::control(chanend_t c_control) {
  (void)c_control;
  xassert(0 && "Sensor Exception: Make sure your control() method is implemented and called from the derived class");
}
