#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <timer.h>

#include "i2c.h"
#include "sensor.h"

#include "sensor_control.h"

void sensor_start(client interface i2c_master_if i2c){
  int r = 0;
  r |= sensor_initialize(i2c);
  delay_milliseconds(100);
  r |= sensor_configure(i2c);
  delay_milliseconds(600);
  r |= sensor_stream_start(i2c);
  assert((r == 0)); // assert that camera is started and configured
}

void sensor_control(
    server interface sensor_control_if sc,
    client interface i2c_master_if i2c)
{
  while(1){
    select {
      
      case sc.set_exposure(unsigned exposure):
        sensor_set_exposure(i2c, exposure);
        break;

      case sc.stop():
        sensor_stream_stop(i2c);
        break;
    }
  }
}

void sensor_control_set_exposure(
    client interface sensor_control_if sc,
    unsigned exposure)
{
  sc.set_exposure(exposure);
}

void sensor_control_stop(
    client interface sensor_control_if sc)
{
  sc.stop();
}
