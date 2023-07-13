
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include <xs1.h>

#include "i2c.h"

#include "sensor.h"
#include "imx219_reg.h"
#include "imx219_i2c.h"

static
const imx219_settings_t STOP_ON = IMX219_END_SETTINGS;


uint16_t imx219_i2c_read_reg16(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    uint16_t reg,
    int* result)
{
  i2c_regop_res_t res;
  uint16_t v = i2c_client.read_reg16(IMX219_I2C_ADDR,
                                     reg,
                                     res);
  *result = res;
  return v;
}
 
int imx219_i2c_write(
    CLIENT_INTERFACE(i2c_master_if, i2c_client), 
    int reg, 
    int value)
{
  i2c_regop_res_t result;
  // Write an 8-bit register on a slave device from a 16-bit register address.
  result = i2c_client.write_reg8_addr16(IMX219_I2C_ADDR, reg, value);
  if (result != I2C_REGOP_SUCCESS)
  {
    printf("Failed on address %02x value %02x\n", reg, value);
    assert(0);
  }
  return result != I2C_REGOP_SUCCESS ? -1 : 0;
}
 
int imx219_i2c_write_table(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    const imx219_settings_t table[]) 
{
	int ret;

  for(int i = 0; 1; i++) {
    uint32_t address = table[i].addr;
    uint32_t value   = table[i].val;

    if(   address == STOP_ON.addr
       && value   == STOP_ON.val) {
      break;
    }

    if (address == SLEEP) {
      timer tmr;
      int t;
      tmr :> t;
      tmr when timerafter(t + TRSTUS * 100) :> void;
    }
    if (address & 0x8000) {
      address &= 0x7fff;
      ret  = imx219_i2c_write(i2c_client, address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
      ret |= imx219_i2c_write(i2c_client, address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4
    } else {
      ret = imx219_i2c_write(i2c_client, address, value);
    }

    if (ret < 0) {
      return ret;
    }
	}
	return 0;
}
 
int imx219_i2c_write_table_val(
    CLIENT_INTERFACE(i2c_master_if, i2c_client),
    const imx219_settings_t table[]) 
{
	int ret;
  char mode = 's';

  for(int i = 0; 1; i++) {
    uint32_t address = table[i].addr;
    uint32_t value   = table[i].val;
    
    if(   address == STOP_ON.addr
       && value   == STOP_ON.val) {
      break;
    }

    if (address == SLEEP) {
      timer tmr;
      int t;
      tmr :> t;
      tmr when timerafter(t + TRSTUS * 100) :> void;
    }

    if ((value & 0xFF00) || (address & 0x8000)){
      if (address & 0x8000) {
        address &= 0x7fff;
      }
      mode = 'c';
      //printf("mode=%c , address  = 0x%04x, value = 0x%02x\n", mode, address, value >> 8);
      //printf("mode=%c , address+ = 0x%04x, value = 0x%02x\n", mode, address+1, value & 0xff);   
      // continous writte
      ret  = imx219_i2c_write(i2c_client, address,   value >> 8); // B1 B2 B3 B4 -> B1 B2
      ret |= imx219_i2c_write(i2c_client, address+1, value & 0xff); // B1 B2 B3 B4 -> B3 B4 
    }
    else
    {
      // single writte
      mode = 's';
      ret = imx219_i2c_write(i2c_client, address, value);
      //printf("mode=%c , address = 0x%04x, value = 0x%02x\n", mode, address, value);
    }    

    if (ret < 0) {
      return ret;
    }
	}
	return 0;
}


