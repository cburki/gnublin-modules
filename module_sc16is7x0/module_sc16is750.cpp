// module_sc16is750.cpp --- 
// 
// Filename     : module_sc16is750.cpp
// Description  : Class for accessing gnublin SC16IS750 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sun Aug 31 16:15:57 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 860
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

#include "module_sc16is750.h"

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(void) 
    : gnublin_module_sc16is7x0() {

    _ioLatchReg = 0x00;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address and the default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(int address)
    : gnublin_module_sc16is7x0(address) {

    _ioLatchReg = 0x00;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address to 0x20 and the given i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(int address, std::string filename)
    : gnublin_module_sc16is7x0(address, filename) {

    _ioLatchReg = 0x00;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Initialize the sc16is750 chipset. FIFO is enabled.
 */
int gnublin_module_sc16is750::init(void) {

    gnublin_module_sc16is7x0::softReset();

    gnublin_module_sc16is7x0::initUART(CONF_INT_DFLT);
    initIO(CONF_IO_DFLT);

    /* Enable the transmit and receive FIFO. */
    gnublin_module_sc16is7x0::enableFifo(1);

    return 1;
}


/**
 * @~english
 * @brief Initialize the GPIOs.
 *
 * @param value The configuration value to initialize the GPIOs.
 */
int gnublin_module_sc16is750::initIO(unsigned char value) {
    
    _errorFlag = false;
    unsigned char txBuf[1];

    /* Set the IOCTRL (I/O Control Register).
     * IOCTRL[0] : I/O latch disabled        -> 0
     * IOCTRL[1] : GPIO[7:4] behave I/O pins -> 0
     * IOCTRL[2] : Reserved
     * IOCTRL[3] : Software reset            -> 0
     * IOCTRL[4] : Reserved
     * IOCTRL[5] : Reserved
     * IOCTRL[6] : Reserved
     * IOCTRL[7] : Reserved
     */
    txBuf[0] = CONF_IO_DFLT | value;
    if (_i2c.send(IOCTRL, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (IOCTRL) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the mode of the given pin.
 *
 * @param pin The pin for which to set the mode.
 * @param direction The direction (in or out) to set to the pin.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is750::pinMode(int pin, std::string direction) {

    _errorFlag = false;
    unsigned char txBuf[1];
    unsigned char rxBuf[1];

    if (pin < 0 || pin > 7) {
        _errorFlag = true;
        _errorMessage = "Pin number is not between 0 and 7\n";
        return -1;
    }

    txBuf[0] = 1 << pin;

    /* Read the current state. */
    if (_i2c.receive(IODIR, rxBuf, 1) > 0) {

        if (direction == OUTPUT) {
            txBuf[0] = rxBuf[0] | txBuf[0];
        }
        else if (direction == INPUT) {
            txBuf[0] = rxBuf[0] & ~txBuf[0];
        }
        else {
            _errorFlag = true;
            _errorMessage = "direction != in/out\n";
            return -1;
        }

        if (_i2c.send(IODIR, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (IODIR) Error\n";
            return -1;
        }
    }

    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IODIR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the mode of the I/O port. All pins of the port are set with
 * the same mode.
 *
 * @param direction The direction (in or out) to set to the I/O port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is750::portMode(std::string direction) {

    _errorFlag = false;
    unsigned char txBuf[1];

    if (direction == OUTPUT) {
        txBuf[0] = 0xff;
    }
    else if (direction == INPUT) {
        txBuf[0] = 0x00;
    }
    else {
        _errorFlag = true;
        _errorMessage = "direction != in/out\n";
        return -1;
    }

    if (_i2c.send(IODIR, txBuf, 1) > 0) {
        return 1;
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.send (IODIR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
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
int gnublin_module_sc16is750::digitalWrite(int pin, int value) {

    _errorFlag = false;
    unsigned char txBuf[1];
    unsigned char rxBuf[1];

    if (pin < 0 || pin > 7) {
        _errorFlag = true;
        _errorMessage = "Pin number is not between 0 and 7\n";
        return -1;
    }

    txBuf[0] = 1 << pin;

    /* Read the current state. */
    if (_i2c.receive(IOSTATE, rxBuf, 1) > 0) {
            
        if (value == 0) {
            txBuf[0] = rxBuf[0] & ~txBuf[0];
        }
        else if (value == 1) {
            txBuf[0] = rxBuf[0] | txBuf[0];
        }
        else {
            _errorFlag = true;
            _errorMessage = "value != 0/1\n";
            return -1;
        }
        
        if (_i2c.send(IOSTATE, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (IOSTATE) Error\n";
            return -1;
        }
    }
    
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOSTATE) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the digital value of the given pin.
 *
 * @param pin The pin for which to read the digital value.
 * @return The value read from the pin.
 */
int gnublin_module_sc16is750::digitalRead(int pin) {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (pin < 0 || pin > 7) {
        _errorFlag = true;
        _errorMessage = "Pin number is not between 0 and 7\n";
        return -1;
    }

    if (_i2c.receive(IOSTATE, rxBuf, 1) > 0) {
        rxBuf[0] <<= (7 - pin);  /* MSB is now the pin we want to read from. */
        rxBuf[0] &= 128;         /* Set all bits to 0 except the MSB. */

        if (rxBuf[0] == 0) {
            return 0;
        }
        else if (rxBuf[0] == 128) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "bitshift failed\n";
            return -1;
        }
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOSTATE) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the state (high/1 or low/0) of the given pin.
 *
 * @param pin The pin for which to read the state.
 * @return The state of the pin. Could be 1 for high or 0 for low.
 */
int gnublin_module_sc16is750::readState(int pin) {

    return digitalRead(pin);
}


/**
 * @~english
 * @brief Write a value (byte) to the I/O port.
 *
 * @param value The byte to write to the I/O port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is750::writePort(unsigned char value) {

    _errorFlag = false;
    unsigned char txBuf[1];
    txBuf[0] = value;

    if (_i2c.send(IOSTATE, txBuf, 1) > 0) {
        return 1;
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.send (IOSTATE) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Read a value (byte) from the I/O port.
 *
 * @return The value read from the I/O port.
 */
unsigned char gnublin_module_sc16is750::readPort() {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (_i2c.receive(IOSTATE, rxBuf, 1) > 0) {
        return rxBuf[0];
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOSTATE) Error";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Enable or disable the interrupt of the given pin.
 *
 * @param pin The pin for which to enable or disable the interrupt.
 * @param value Enable or disable value. 1 is enable and 0 is disable.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is750::pinIntEnable(int pin, int value) {

    _errorFlag = false;
    unsigned char txBuf[1];
    unsigned char rxBuf[1];

    if (pin < 0 || pin > 7) {
        _errorFlag = true;
        _errorMessage = "Pin number is not between 0 and 7\n";
        return -1;
    }

    txBuf[0] = 1 << pin;

    /* Read the current state. */
    if (_i2c.receive(IOINTEN, rxBuf, 1) > 0) {
            
        if (value == 0) {
            txBuf[0] = rxBuf[0] & ~txBuf[0];
        }
        else if (value == 1) {
            txBuf[0] = rxBuf[0] | txBuf[0];
        }
        else {
            _errorFlag = true;
            _errorMessage = "value != 0/1\n";
            return -1;
        }
        
        if (_i2c.send(IOINTEN, txBuf, 1) > 0) {

            /* Store the IOSTATE value fro the pin. */
            int pinState = digitalRead(pin);
            if (pinState == 0) {
                _ioLatchReg &= ~(pinState << pin);
            }
            else if (pinState == 1) {
                _ioLatchReg |= (pinState << pin);
            }
            else {
                return -1;
            }

            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (IOINTEN) Error\n";
            return -1;
        }
    }
    
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOINTEN) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Enable or disable the interrupts of the I/O port.
 *
 * @param value Enable or disable value. 1 is enable and 0 is disable.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is750::portIntEnable(int value) {

    _errorFlag = false;
    unsigned char txBuf[1];

    if (value == 0) {
        txBuf[0] = 0x00;
    }
    else if (value == 1) {
        txBuf[0] = 0xff;
    }
    else {
        _errorFlag = true;
        _errorMessage = "value != 0/1\n";
        return -1;
    }

    if (_i2c.send(IOINTEN, txBuf, 1) > 0) {

        /* Store the IOSTATE value fro the pin. */
        _ioLatchReg = readPort();
        if (fail()) {
            _ioLatchReg = 0x00;
            return -1;
        }

        return 1;
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.send (IOINTEN) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english 
 * @brief Read the interrupt flag on the I/O port. This will return on
 * which pins interrupt occurs. Note that because we are reading the
 * IOSTATE, the interrupt is cleared.
 *
 * @return The interrupt flags on the port or -1 on error.
 */
unsigned char gnublin_module_sc16is750::readIntFlagPort(void) {

    _errorFlag = false;
    unsigned char ioDir[1];
    unsigned char intEn[1];
    unsigned char ioState[1];

    ioState[0] = readPort();
    if (_errorFlag) {  /* Error while reading port state. */
        return -1;
    }

    if (_i2c.receive(IODIR, ioDir, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IODIR) Error\n";
        return -1;
    }
    
    if (_i2c.receive(IOINTEN, intEn, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOITNEN) Error\n";
        return -1;
    }

    unsigned char state = ioState[0] & ~ioDir[0] & intEn[0];
    unsigned char latch = _ioLatchReg & ~ioDir[0] & intEn[0];
    _ioLatchReg = ioState[0];

    return state ^ latch;
}


/**
 * @~english
 * @brief Poll for an interrupt. It call the appropriate ISR callbacks when
 * an interrupt occurs.
 *
 * @return The number of interrupts or -1 on error.
 */
int gnublin_module_sc16is750::pollInt(void) {

    _errorFlag = false;

    int count = 0;
    int interrupt = whichInt();
    unsigned char intFlags;

    if (interrupt == 0) {
        /* No pending interrupt. */
        return interrupt;
    }
    if (interrupt < 0) {
        /* Error while identifying interrupt. */
        return interrupt;
    }
    //printf("interrupt=0x%02x\n", interrupt);

    switch (interrupt) {
    case INT_RLS :  /* Receiver line status error. */
        break;
    case INT_RTOUT :  /* Receiver timeout. */
        break;
    case INT_RHR :  /* RHR. */
        if (_isrDataReceived != NULL) {
            int available = rxAvailableData();
            char *buffer = (char *)malloc(available + 1);
            read(buffer, available);
            buffer[available] = '\0';
            
            _isrDataReceived(buffer, available + 1);
        }
        count++;
        break;
    case INT_THR :  /* THR. */
        if (_isrSpaceAvailable != NULL) {
            int available = txAvailableSpace();
            
            _isrSpaceAvailable(available);
        }
        count++;
        break;
    case INT_MODEM :  /* Modem. */
        break;
    case INT_PINS :  /* Input pin change. */
        intFlags = readIntFlagPort();
        //printf("intFlags=0x%02x\n", (unsigned int)intFlags);

        for (int pin = 0; pin < 8; pin++) {
            if (intFlags & (1 << pin)) {

                /* The value of the pin has changed. */
                int value = _ioLatchReg << (7 - pin);
                value &= 128;
                if (value == 128) {
                    value = 1;
                }
                
                if (_isrIO != NULL) {
                    _isrIO(pin, value);
                }
                
                count++;
            }
        }

        /* Store the IO values in case they have changed before the interrupts
           have been treated. */
        _ioLatchReg = readPort();
        break;
    case INT_XOFF :  /* Received Xoff signal / special character. */
        break;
    case INT_CTSRTS :  /* CTS, RTS change of state from active (LOW) to inactive (HIGH). */
        break;
    default :
        _errorFlag = true;
        _errorMessage = "Unknown interrupt source\n";
        return -1;
        break;
    }

    return count;
}


/**
 * @~english
 * @brief Register a global Interrupt Service Routine. It will be called when
 * an interrupt occurs on any pins.
 *
 * @param isr Callback function that will be called on interrupt.
 *
 * isr(int pin, int value)
 */
int gnublin_module_sc16is750::intIsrIO(void (*isr)(int, int)) {

    _isrIO = isr;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_sc16is750.cpp ends here
