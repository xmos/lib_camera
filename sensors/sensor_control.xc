#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#include "i2c.h"
#include "sensor_control.h"
#include "sensor.h"

void sensor_control(
    server interface sensor_control_if sc,
    client interface i2c_master_if i2c)
{
  while(1){
    select {
      case sc.set_exposure(unsigned exposure):
        camera_set_exposure(i2c, exposure);
        break;
      case sc.stop():
        printf("exiting sensor_control thread\n");
        return;
    }
  }
}


void sensor_control_set_exposure(
    client interface sensor_control_if sc,
    unsigned exposure)
{
  sc.set_exposure(exposure);
}

void sensor_stop(
    CLIENT_INTERFACE(sensor_control_if, sc))
{
  sc.stop();
}