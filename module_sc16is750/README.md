Summary
-------

This module support the SC16IS750 chip. The SC16IS750 is a slave I2C-bus/SPI interface to a single-channel high performance UART. It also provide 8 additional programmable I/O pins. I use this chip in a Gnublin extension to allow communicating over the air using a XBee RF module.

Note that this module only support the I2C interface. There is no support in the code for the SPI interface.


Installation
------------

See the README file of the upper directory for installation instructions.


Code Samples
------------

The first example show how to send a message to another XBee module and wait for incoming data.

    #include "module_sc16is750.h"

    int main(void) {
        gnublin_module_sc16is750 xbee(0x4d);
        xbee.init();
        xbee.setBaudRate(UART_9600);

        /* Send a message */
        xbee.write("Hello Word !\n");

        /* Always read */
        int available = 0;
        while (1) {
            if ((available = xbee.rxAvailableData()) > 0) {
                char buffer[available + 1];
                xbee.read(buffer, available);
                buffer[available + 1] = '\0';
            }
        }
    
        return 1;
    }

The sample below show how to set a GPIO pin to HIGH.

    #include "module_sc16is750.h"

    int main(void) {
        gnublin_module_sc16is750 gpio(0x4d);
        gpio.init();
    
        /* Set the pin 0 to output and then high */
        gpio.pinMode(0, OUTPUT);
        gpio.digitalWrite(0, HIGH);
    
        return 1;
    }
