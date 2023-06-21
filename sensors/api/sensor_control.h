
#pragma once

#include "xccompat.h"


#ifdef __XC__

typedef interface sensor_control_if {
    void set_exposure(unsigned exposure);
    void stop();
} sensor_control_if;

void sensor_control(
    server interface sensor_control_if sc,
    client interface i2c_master_if i2c);

void sensor_startup(client interface i2c_master_if i2c);

#endif

/* These wrappers are for calling client interface functions from C */
void sensor_control_set_exposure(
    CLIENT_INTERFACE(sensor_control_if, sc),
    const unsigned exposure
    );

void sensor_control_stop(
    CLIENT_INTERFACE(sensor_control_if, sc));
