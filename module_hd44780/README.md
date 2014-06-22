Summary
-------

This module support characters LCDs based on the HD44780 chip. The LCD is controlled through a driver. Here is the supported drivers.

 - GPIO : The LCD is directly connected to the GPIOs.
 - MCP23017 : The LCD is connected to the GPIOs of a MCP23017 device.


Installation
------------

See the README file of the upper directory for installation instructions.


Code Sample
-----------

Here is a simple example showing how to print a string on the LCD using the MCP23017 driver.

    #include "module_hd44780.h"
    
    int main(void) {
        gnublin_module_driver_mcp23017 driver;
        gnublin_module_hd44780 lcd(&driver);

        lcd.init();
        lcd.print((char *)"Hello Word !");
        return 1;
    }