// module_mcp230xx.cpp --- 
// 
// Filename     : module_mcp230xx.cpp
// Description  : Class for accessing gnublin MCP23017 and MCP23009 port expander.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Sun Apr 27 15:39:20 2014
// Version      : 1.0.0
// Last-Updated : Sun Oct 12 15:51:14 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 51
// URL          : 
// Keywords     : 
// Compatibility: 
// 
// 

// Commentary   : 
// 
// 
// 
// 

// Change log:
// 
// 
// 
// 

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; see the file LICENSE.  If not, write to the
// Free Software Foundation, Inc., 51 Franklin Street, Fifth
// ;; Floor, Boston, MA 02110-1301, USA.
// 
// 

// Code         :

/* -------------------------------------------------------------------------- */

#include "module_mcp230xx.h"

/* -------------------------------------------------------------------------- */

#define GPA        0     /* Port A */
#define GPB        1     /* Port B */

/* Register addresses are given for the MCP23017. They must be divided by 2
   (shift right by 1) when accessing the registers for the MCP23009. */
#define IODIRA     0x00  /* IO Direction Register A */
#define IODIRB     0x01  /* IO Direction Register B */
#define IPOLA      0x02  /* Input Polarity Register A */
#define IPOLB      0x03  /* Input Polarity Register B */
#define GPINTENA   0x04  /* Interrupt Enable Register A */
#define GPINTENB   0x05  /* Interrupt Enable Register B */
#define DEFVALA    0x06  /* Default Compare Register for Interrupt-On-Change A */
#define DEFVALB    0x07  /* Default Compare Register for Interrupt-On-Change A */
#define INTCONA    0x08  /* Interrupt Control Register A */
#define INTCONB    0x09  /* Interrupt Control Register B */
#define IOCON      0x0A  /* Configuration Register */
#define GPPUA      0x0C  /* Pull-Up Resistor Register A */
#define GPPUB      0x0D  /* Pull-Up Resistor Register B */
#define INTFA      0x0E  /* Interrupt Flag Register A */
#define INTFB      0x0F  /* Interrupt Flag Register B */
#define INTCAPA    0x10  /* Interrupt Capture Register A */
#define INTCAPB    0x11  /* Interrupt Capture Register B */
#define GPIOA      0x12  /* Port Register A */
#define GPIOB      0x13  /* Port Register B */
#define OLATA      0x14  /* Output Latch Register A */
#define OLATB      0x15  /* Output Latch Register B */

#define CONF_SEQOP 0x20  /* Sequential Operation Mode */

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 *
 * @param ports The number of ports of the device.
 * @param pins The number of pins of the device.
 * @param address The i2c address.
 * @param filename The i2c device file.
 */
gnublin_module_mcp230xx::gnublin_module_mcp230xx(int ports, int pins, int address, std::string filename) {

    errorFlag = false;
    this->ports = ports;
    this->pins = pins;
    registerShift = 0;
    if (this->ports == 1) {
        registerShift = 1;
    }

    setAddress(address);
    setDevicefile(filename);
    init(CONF_SEQOP);

    isr = NULL;
    for (int i = 0; i < MAX_PINS; i++) {
        pinIsr[i] = NULL;
    }
    for (int i = 0; i < MAX_PORTS; i++) {
        portIsr[i] = NULL;
    }
}


/**
 * @~english
 * @brief Initialize the MCP23017 or MCP23009.
 *
 * @value The initialization value.
 * @return -1 on error and 0 on success.
 */
int gnublin_module_mcp230xx::init(unsigned char value) {

    value |= CONF_SEQOP;  /* Always enable seq mode. */

    if (i2c.send(IOCON >> registerShift, &value, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    /* Disable interrupts on both ports. */
    if ((portIntMode(0, INT_NONE) < 0)
        || (portIntMode(1, INT_NONE) < 0)) {
        errorFlag = true;
        errorMessage = "disable interrupts Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Get the last error message.
 *
 * @return The error message as c-string.
 */
const char* gnublin_module_mcp230xx::getErrorMessage(void) {

    return errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_mcp230xx::fail(void) {

    return errorFlag;
}


/**
 * @~english
 * @brief Set the i2c address.
 *
 * @param address The address to set.
 */
void gnublin_module_mcp230xx::setAddress(int address) {

    i2c.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file.
 *
 * @param filename The i2c device filename.
 */
void gnublin_module_mcp230xx::setDevicefile(std::string filename) {

    i2c.setDevicefile(filename);
}


/**
 * @~english
 * @brief Set the mode of the given pin.
 *
 * @param pin The pin for which to set the mode.
 * @param direction The direction (in or out) to set to the pin.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::pinMode(int pin, std::string direction) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char rxValue;
    int registerAddress;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerAddress = IODIRA  >> registerShift;
        txValue = 1 << pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerAddress = IODIRB;
        txValue = 1 << (pin - 8);
    }
    else {
        return -1;
    }

    /* Read the current state. */
    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
            
        if (direction == OUTPUT) {
            txValue = rxValue & ~txValue;
        }
        else if (direction == INPUT) {
            txValue = rxValue | txValue;
        }
        else {
            errorFlag = true;
            errorMessage = "direction != in/out\n";
            return -1;
        }
        
        if (i2c.send(registerAddress, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send Error\n";
            return -1;
        }
    }
    
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the mode of the given port. All pins of the port are set with
 * the same mode.
 *
 * @param port The port for which to set the mode.
 * @param direction The direction (in or out) to set to the port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::portMode(int port, std::string direction) {

    errorFlag = false;
    unsigned char txValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = IODIRA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = IODIRB;
    }
    else {
        return -1;
    }

    if (direction == OUTPUT) {
        txValue = 0x00;
    }
    else if (direction == INPUT) {
        txValue = 0xff;
    }
    else {
        errorFlag = true;
        errorMessage = "direction != in/out\n";
        return -1;
    }
    
    if (i2c.send(registerAddress, &txValue, 1) > 0) {
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Write a digital value to the given pin.
 *
 * @param pin The pin to which to write a digital value.
 * @param value The digital value to write to the pin.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::digitalWrite(int pin, int value) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char rxValue;
    int registerAddress;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerAddress = OLATA >> registerShift;
        txValue = 1 << pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerAddress = OLATB;
        txValue = 1 << (pin - 8);
    }
    else {
        return -1;
    }

    /* Read the current state. */
    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
            
        if (value == 0) {
            txValue = rxValue & ~txValue;
        }
        else if (value == 1) {
            txValue = rxValue | txValue;
        }
        else {
            errorFlag = true;
            errorMessage = "value != 0/1\n";
            return -1;
        }
        
        if (i2c.send(registerAddress, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send Error\n";
            return -1;
        }
    }
    
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the digital value of the given pin.
 *
 * @param pin The pin for which to read the digital value.
 * @return The value read from the pin.
 */
int gnublin_module_mcp230xx::digitalRead(int pin) {

    errorFlag = false;
    unsigned char rxValue;
    int registerAddress;
    int shift;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerAddress = GPIOA >> registerShift;
        shift = 7 - pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerAddress = GPIOB;
        shift = 15 - pin;
    }
    else {
        return -1;
    }

    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
        rxValue <<= shift;  /* MSB is now the pin we want to read from. */
        rxValue &= 128;     /* Set all bits to 0 except the MSB. */

        if (rxValue == 0) {
            return 0;
        }
        else if (rxValue == 128) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "bitshift failed\n";
            return -1;
        }
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the state (high/1 or low/0) of the given pin.
 *
 * @param pin The pin for which to read the state.
 * @return The state of the pin. Could be 1 for high or 0 for low.
 */
int gnublin_module_mcp230xx::readState(int pin) {

    return digitalRead(pin);
}


/**
 * @~english
 * @brief Write a value (byte) to the given port.
 *
 * @param port The port to which to write a value.
 * @param value The byte to write to the port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::writePort(int port, unsigned char value) {

    errorFlag = false;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = OLATA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = OLATB;
    }
    else {
        return -1;
    }

    if (i2c.send(registerAddress, &value, 1) > 0) {
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read a value (byte) from the given port.
 *
 * @param port The port from which to read the value.
 * @return The value read from the port.
 */
unsigned char gnublin_module_mcp230xx::readPort(int port) {

    errorFlag = false;
    unsigned char rxValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = GPIOA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = GPIOB;
    }
    else {
        return -1;
    }

    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
        return rxValue;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the interrupt mode of the given pin.
 *
 * @param pin The pin for which to set the interrupt mode.
 * @param mode The mode for the interrupt which could be change, high, low or none.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::pinIntMode(int pin, std::string mode) {

    errorFlag = false;
    unsigned char rxIntEn;
    unsigned char rxDefVal;
    unsigned char rxIntCon;
    unsigned char txIntEn;
    unsigned char txDefVal;
    unsigned char txIntCon;
    int registerIntEn;
    int registerDefVal;
    int registerIntCon;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerIntEn = GPINTENA >> registerShift;
        registerDefVal = DEFVALA >> registerShift;
        registerIntCon = INTCONA >> registerShift;
        txIntEn = 1 << pin;
        txDefVal = 1 << pin;
        txIntCon = 1 << pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerIntEn = GPINTENB;
        registerDefVal = DEFVALB;
        registerIntCon = INTCONB;
        txIntEn = 1 << (pin - 8);
        txDefVal = 1 << (pin - 8);
        txIntCon = 1 << (pin - 8);
    }
    else {
        return -1;
    }

    /* Read the current states. */
    if (i2c.receive(registerDefVal, &rxDefVal, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    if (i2c.receive(registerIntCon, &rxIntCon, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    if (i2c.receive(registerIntEn, &rxIntEn, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    if (mode == INT_CHANGE) {
        txDefVal = rxDefVal & ~txDefVal;
        txIntCon = rxIntCon & ~txIntCon;
        txIntEn = rxIntEn | txIntEn;
    }
    else if (mode == INT_HIGH) {
        txDefVal = rxDefVal & ~txDefVal;
        txIntCon = rxIntCon | txIntCon;
        txIntEn = rxIntEn | txIntEn;
    }
    else if (mode == INT_LOW) {
        txDefVal = rxDefVal | txDefVal;
        txIntCon = rxIntCon | txIntCon;
        txIntEn = rxIntEn | txIntEn;
    }
    else if (mode == INT_NONE) {
        txDefVal = rxDefVal & ~txDefVal;
        txIntCon = rxIntCon & ~txIntCon;
        txIntEn = rxIntEn & ~txIntEn;
    }
    else {
        errorFlag = true;
        errorMessage = "mode != change/high/low/none\n";
        return -1;
    }

    if (i2c.send(registerDefVal, &txDefVal, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    if (i2c.send(registerIntCon, &txIntCon, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    if (i2c.send(registerIntEn, &txIntEn, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the interrupt mode of the given port.
 *
 * @param port The port for which to set the mode.
 * @param value The mode for the interrupt which could be change, high, low or none.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::portIntMode(int port, std::string mode) {

    errorFlag = false;
    unsigned char txIntEn;
    unsigned char txDefVal;
    unsigned char txIntCon;
    int registerIntEn;
    int registerDefVal;
    int registerIntCon;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerIntEn = GPINTENA >> registerShift;
        registerDefVal = DEFVALA >> registerShift;
        registerIntCon = INTCONA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerIntEn = GPINTENB;
        registerDefVal = DEFVALB;
        registerIntCon = INTCONB;
    }
    else {
        return -1;
    }

    if (mode == INT_CHANGE) {
        txDefVal = 0x00;
        txIntCon = 0x00;
        txIntEn = 0xff;
    }
    else if (mode == INT_HIGH) {
        txDefVal = 0x00;
        txIntCon = 0xff;
        txIntEn = 0xff;
    }
    else if (mode == INT_LOW) {
        txDefVal = 0xff;
        txIntCon = 0xff;
        txIntEn = 0xff;
    }
    else if (mode == INT_NONE) {
        txDefVal = 0x00;
        txIntCon = 0x00;
        txIntEn = 0x00;
    }
    else {
        errorFlag = true;
        errorMessage = "mode != change/high/low/none\n";
        return -1;
    }

    if (i2c.send(registerDefVal, &txDefVal, 1) > 0) {
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    if (i2c.send(registerIntCon, &txIntCon, 1) > 0) {
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    if (i2c.send(registerIntEn, &txIntEn, 1) > 0) {
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the pull-up resistor mode of the given pin.
 *
 * @param pin The pin for which to set the pull-up resistor mode.
 * @param value The value (0 or 1) to set or clear the pull-up mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::pinPullUpMode(int pin, int value) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char rxValue;
    int registerAddress;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerAddress = GPPUA >> registerShift;
        txValue = 1 << pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerAddress = GPPUB;
        txValue = 1 << (pin - 8);
    }
    else {
        return -1;
    }

    /* Read the current state. */
    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
            
        if (value == 0) {
            txValue = rxValue & ~txValue;
        }
        else if (value == 1) {
            txValue = rxValue | txValue;
        }
        else {
            errorFlag = true;
            errorMessage = "value != 0/1\n";
            return -1;
        }
        
        if (i2c.send(registerAddress, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send Error\n";
            return -1;
        }
    }
    
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the pull-up resistor mode (enable or disable) of the given port.
 * All pins of the port are set with the same value.
 *
 * @param port The port for which to set the pull-up resistor mode.
 * @param value The value to set or clear the pull-up mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::portPullUpMode(int port, int value) {

    errorFlag = false;
    unsigned char txValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = GPPUA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = GPPUB;
    }
    else {
        return -1;
    }

    if (value == 0) {
        txValue = 0x00;
    }
    else if (value == 1) {
        txValue = 0xff;
    }
    else {
        errorFlag = true;
        errorMessage = "value != 0/1\n";
        return -1;
    }

    if (i2c.send(registerAddress, &txValue, 1) > 0) {
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the input polarity mode of the given pin. If the mode is set, the
 * GPIO will reflect the inverted value of the input pin.
 *
 * @param pin The pin for which to set the input polarity mode.
 * @param value The value (0 or 1) to set or clear the polarity mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::pinPolarityMode(int pin, int value) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char rxValue;
    int registerAddress;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        registerAddress = IPOLA >> registerShift;
        txValue = 1 << pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        registerAddress = IPOLB;
        txValue = 1 << (pin - 8);
    }
    else {
        return -1;
    }

    /* Read the current state. */
    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
            
        if (value == 0) {
            txValue = rxValue & ~txValue;
        }
        else if (value == 1) {
            txValue = rxValue | txValue;
        }
        else {
            errorFlag = true;
            errorMessage = "value != 0/1\n";
            return -1;
        }
        
        if (i2c.send(registerAddress, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send Error\n";
            return -1;
        }
    }
    
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the input polarity mode (enable or disable) of the given port. If the pin
 * is set, the GPIO will reflect the inverted value of the input pin.
 * All pins of the port are set with the same value.
 *
 * @param port The port for which to set the input polarity mode.
 * @param value The value to set or clear the polarity mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp230xx::portPolarityMode(int port, int value) {

    errorFlag = false;
    unsigned char txValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = IPOLA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = IPOLB;
    }
    else {
        return -1;
    }

    if (value == 0) {
        txValue = 0x00;
    }
    else if (value == 1) {
        txValue = 0xff;
    }
    else {
        errorFlag = true;
        errorMessage = "value != 0/1\n";
        return -1;
    }

    if (i2c.send(registerAddress, &txValue, 1) > 0) {
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the digital value of the given pin at the time the
 * interrupt occurs.
 *
 * @param pin The pin for which to read the digital value at the time the interrupt occurs.
 * @return The value read from the pin.
 */
int gnublin_module_mcp230xx::digitalIntRead(int pin) {

    errorFlag = false;
    unsigned char rxValue;
    int port;
    int registerAddress;
    int shift;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    if (pin >= 0 && pin <= 7) {  /* Port A */
        port = 0;
        registerAddress = INTCAPA >> registerShift;
        shift = 7 - pin;
    }
    else if (pin >= 8 && pin <= 15) {  /* Port B */
        port = 1;
        registerAddress = INTCAPB;
        shift = 15 - pin;
    }
    else {
        return -1;
    }

    /* Check if an interrupt occurs. */
    unsigned char intFlags = readIntFlagPort(port);
    if (intFlags == 0) {
        printf("No interrupt on port %d\n", port);
        return 0;
    }
    else if (intFlags < 0) {
        return -1;
    }

    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
        rxValue <<= shift;  /* MSB is now the pin we want to read from. */
        rxValue &= 128;     /* Set all bits to 0 except the MSB. */

        if (rxValue == 0) {
            return 0;
        }
        else if (rxValue == 128) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "bitshift failed\n";
            return -1;
        }
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the interrupt capture for the given port. This will clear
 * the interrupt on the port.
 *
 * @param port The port from which to read the interrupt capture.
 * @return The value read on the port at the time the interrupt occurs or -1 on error.
 */
unsigned char gnublin_module_mcp230xx::readIntPort(int port) {

    errorFlag = false;
    unsigned char rxValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = INTCAPA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = INTCAPB;
    }
    else {
        return -1;
    }

    /* Check if an interrupt occurs. */
    unsigned char intFlags = readIntFlagPort(port);
    if (intFlags == 0) {
        printf("NO interrupt on port A\n");
        return 0;
    }
    else if (intFlags < 0) {
        return -1;
    }

    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
        return rxValue;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the interrupt flag for the given port. This will return on which
 * pins interrupt occurs.
 *
 * @param port The port from which to read the interrupt flag.
 * @return The interrupt flags of the port or -1 on error.
 */
unsigned char gnublin_module_mcp230xx::readIntFlagPort(int port) {

    errorFlag = false;
    unsigned char rxValue;
    int registerAddress;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    if (port == GPA) {  /* Port A */
        registerAddress = INTFA >> registerShift;
    }
    else if (port == GPB) {  /* Port B */
        registerAddress = INTFB;
    }
    else {
        return -1;
    }

    if (i2c.receive(registerAddress, &rxValue, 1) > 0) {
        return rxValue;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive Error";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Poll for an interrupt. It call the appropriate ISR callbacks when
 * an interrupt occurs.
 *
 * It does not really poll. This method can be called when a poll on the
 * pins connected to INTA or INTB is made from the main program so that
 * ISR are called when interrupts occurs.
 *
 * @return The number of interrupts that occurs since last poll.
 */
int gnublin_module_mcp230xx::pollInt(void) {

    int count = 0;

    for (int port = 0; port < MAX_PORTS; port++ ) {
        unsigned char intFlags = readIntFlagPort(port);

        if (intFlags == 0) {
            /* No interrupts. */
            continue;
        }

        for (int pin = 0; pin < 8; pin++) {
            if (intFlags & (1 << pin)) {
                /* Read the value from interrupt register so that it will clear the interrupt. */
                int value = digitalIntRead(pin + (port * 8));

                if (isr != NULL) {
                    isr(port, pin, value);
                }

                if (portIsr[port] != NULL) {
                    (*portIsr[port])(pin, value);
                }

                if (pinIsr[pin + (port * 8)] != NULL) {
                    (*pinIsr[pin + (port * 8)])(value);
                }

                count++;
            }
        }
    }

    return count;
}


/**
 * @~english
 * @brief Register a global Interrupt Service Routine. It will be called when
 * an interrupt occurs on any pins of any ports.
 *
 * @param isr Callback function that will be called on interrupt.
 *
 * isr(int port, int pin, int value)
 */
int gnublin_module_mcp230xx::intIsr(void (*isr)(int, int, int)) {

    isr = isr;
    return 1;
}


/**
 * @~english
 * @brief Register a Interrupt Service Routine for a given pin. The ISR will be
 * called when an interrupt occurs on the given pin.
 *
 * @param isr Callback function that will be called on interrupt.
 *
 * isr(int value)
 */
int gnublin_module_mcp230xx::pinIntIsr(int pin, void (*isr)(int)) {

    errorFlag = false;

    if (pin < 0 || pin > pins - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Pin number is not between 0 and %d\n", pins - 1);
        return -1;
    }

    pinIsr[pin] = isr;
    return 1;
}


/**
 * @~english
 * @brief Register a Interrupt Service Routine for a given port. The ISR will be
 * called when an interrupt occurs on any pin of the given port.
 *
 * @param isr Callback function that will be called on interrupt.
 *
 * isr(int pin, int value)
 */
int gnublin_module_mcp230xx::portIntIsr(int port, void (*isr)(int, int)) {

    errorFlag = false;

    if (port < 0 || port > ports - 1) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Port number is not between 0 and %d\n", ports - 1);
        return -1;
    }

    portIsr[port] = isr;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_mcp230xx.cpp ends here
