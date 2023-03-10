// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

/* App headers */
#include <stdio.h>
#include <platform.h>
#include <xcore/channel.h>
#include <xcore/port.h>
#include <xcore/parallel.h>
#include <xcore/hwtimer.h>
#include <xcore/select.h>
#include <xscope.h>
#include <platform.h>
#include "i2c.h"

#define BUTTON_0_PRESSED 2
#define BUTTON_1_PRESSED 0
#define BUTTONS_NO_PRESSED 3
#define ALL_LEDS_ON 0xf
#define ALL_LEDS_OFF 0

// I2C 
#define DEVICE_ADDR  0x3c
port_t p_scl = XS1_PORT_1A;
port_t p_sda = XS1_PORT_1B; 

I2C_CALLBACK_ATTR
i2c_slave_ack_t i2c_ack_read_req(void *app_data) {
    printstr("xCORE got start of read transaction\n");
    return I2C_SLAVE_ACK;
}

I2C_CALLBACK_ATTR
i2c_slave_ack_t i2c_ack_write_req(void *app_data) {
    printstr("xCORE got start of write transaction\n");
    return I2C_SLAVE_ACK;
}


// Button
port_t button_port = XS1_PORT_4D;    
port_t led_port = XS1_PORT_4C;


void wait_for_button_pressed(port_t button_port, port_t led_port){
    uint8_t counter = 0, rising_edge = 0; 
    unsigned long p_button_state;

    unsigned long button_state = port_in(button_port); // i force to be equal at the beginning
    port_set_trigger_in_not_equal(button_port, button_state); // initial trigger conditiion

    // while loop
    SELECT_RES(
        CASE_THEN(button_port, on_button_change))
    {
    on_button_change : {
        p_button_state = button_state;
        button_state = port_in(button_port);

        // rising edge
        rising_edge = p_button_state == BUTTONS_NO_PRESSED && button_state == BUTTON_0_PRESSED;

        // print some info and tur on leds
        if (rising_edge)
        {
            port_out(led_port, counter++);
            printf("Number of times pressed = %hhu\n", counter);
        }

        // here reset the condition before leaving
        port_set_trigger_in_not_equal(button_port, button_state);
        continue;
        }
    }

}

int main(void)
{


    printf("Wellcome\n");
    port_enable(led_port);
    port_enable(button_port);
    
    wait_for_button_pressed(button_port, led_port);

    port_disable(led_port);
    port_disable(button_port);
    return 0;
}


//https://www.xmos.ai/documentation/XM-014363-PC-6/html/prog-guide/quick-start/c-programming-guide/index.html#basic-port-i-o
