#include <stdint.h>
#include <stdio.h>
#include <xs1.h>
#include "i2c.h"
#include "gc2145.h"

//#define SWIDTH    3296
//#define SHEIGHT   2480

#define GC2145_I2C_ADDR (0x3C)

#define GC2145MIPI_2Lane
#define GC2145MIPI_TEST_PATTERN_CHECKSUM 0x54ddf19b 

void exit(int);
	
static 
int i2c_write(
    client interface i2c_master_if i2c, 
    int reg, 
    int value) 
{
    i2c_regop_res_t result, res2;  
    result = i2c.write_reg(GC2145_I2C_ADDR, reg, value);
    if (result != I2C_REGOP_SUCCESS) {
        printf("Failed on address %02x value %02x\n", reg, value);
        uint8_t val2;
        val2 = i2c.read_reg(GC2145_I2C_ADDR, reg, res2);
        printf("%02x %02x %02x %02x %d %d\n", GC2145_I2C_ADDR, reg, value, val2, res2, result);
    }
    return result != I2C_REGOP_SUCCESS ? (reg == 0xfe ? 0 : -1) : 0;
}

static 
int i2c_write_table(
    client interface i2c_master_if i2c,
    const gc_settings_t table[],
    int N) 
{
	int ret;
    for(int i = 0; i < N; i++) {
        uint32_t address = table[i].addr;
        uint32_t value   = table[i].val;
        if (address == SLEEP) {
            timer tmr;
            int t;
            tmr :> t;
            tmr when timerafter(t + TRSTUS * 100) :> void;
        }
        if (address & 0x8000) {
            address &= 0x7fff;
            ret  = i2c_write(i2c, address,   value >> 8);
            ret |= i2c_write(i2c, address+1, value & 0xff);
        } else {
            ret = i2c_write(i2c, address, value);
        }
		if (ret < 0) {
			return ret;
        }
	}
    for(int i = 240; i < 256; i++) {
    }
	return 0;
}

static
uint8_t i2c_read(
    client interface i2c_master_if i2c,
    unsigned page,
    uint8_t reg_addr)
{
  uint8_t val;
  i2c_regop_res_t res;
  res = i2c.write_reg(GC2145_I2C_ADDR, 0xFE, (page & 0x03));
  if(res) { printf("Register write error. Reg: 0xFE; Page: %u\n", page & 0x03); exit(1); }
  val = i2c.read_reg(GC2145_I2C_ADDR, reg_addr, res);
  if(res) { printf("Register read error. Reg: 0x%02X; Page: %u\n", (unsigned) reg_addr, page & 0x03); exit(1); }
  res = i2c.write_reg(GC2145_I2C_ADDR, 0xFE, 0);
  if(res) { printf("Register write error. Reg: 0xFE; Page: 0\n"); exit(1); }
  return val; 
}

void gc2145_print_info(
    client interface i2c_master_if i2c)
{
  i2c_regop_res_t result;
  uint8_t value;
  unsigned rewq;

#define PRINT_REG(PAGE, REG_ADDR, TEXT)     do { \
    value = i2c_read(i2c, PAGE, REG_ADDR); \
    printf("%s: 0x%02X   (%u)\n", TEXT, (unsigned) value, (unsigned) value); \
  } while(0)

  #define PRINT_WIDE_REG(PAGE, REG_ADDR, TEXT)     do { \
    value = i2c_read(i2c, PAGE, REG_ADDR); \
    rewq = ((unsigned)value) << 8; \
    value = i2c_read(i2c, PAGE, REG_ADDR+1); \
    rewq = (rewq | value); \
    printf("%s: 0x%04X  (%u)\n", TEXT, rewq, rewq); \
  } while(0)

  PRINT_WIDE_REG(0, 0xF0, "Chip ID");
  PRINT_REG(0, 0xFB, "I2C Address");
  PRINT_REG(0, 0xF2, "Pad IO");
  PRINT_WIDE_REG(0, 0x03, "Exposure");
  PRINT_WIDE_REG(0, 0x05, "H Blanking");
  PRINT_WIDE_REG(0, 0x07, "V Blanking");
  PRINT_WIDE_REG(0, 0x0D, "Window Height");
  PRINT_WIDE_REG(0, 0x0F, "Window Width");;
  PRINT_REG(0, 0x83, "Special effect");
  PRINT_REG(0, 0x84, "Output Format");
  PRINT_REG(0, 0x85, "Frame start num");
  PRINT_REG(0, 0x90, "Crop enable");
  PRINT_WIDE_REG(0, 0x91, "out_win.y[1]");
  PRINT_WIDE_REG(0, 0x93, "out_win.x[1]");
  PRINT_WIDE_REG(0, 0x95, "out_win.height");
  PRINT_WIDE_REG(0, 0x97, "out_win.width");

#undef PRINT_REG
}


int gc2145_stream_stop(client interface i2c_master_if i2c) {
	int ret;
  ret = i2c_write(i2c, 0xFE, 0x00);
  ret = ret | i2c_write(i2c, 0xF2, 0x00);
  return ret;
}

int gc2145_stream_start(client interface i2c_master_if i2c) {
	int ret;
  ret = i2c_write(i2c, 0xFE, 0x00);
  ret = ret | i2c_write(i2c, 0xF2, 0x0F);
  return ret;
}

int gc2145_init(
    client interface i2c_master_if i2c) 
{
  int ret;
  printf("gc2145_stream_init()...\n");
  // gc2145_print_info(i2c);


	ret = i2c_write_table(i2c, chip_set_up, chip_set_up_length);
  if(ret)
    printf("Failed to set all GC2145 registers.\n");
    
  i2c_write(i2c, 0x84, 0x6);

  // i2c_write(i2c, 0xFE, 0xC0);

  gc2145_stream_stop(i2c);

  gc2145_print_info(i2c);
  
  return ret;
}

