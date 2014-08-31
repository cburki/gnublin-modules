// module_sc16is7x0.cpp --- 
// 
// Filename     : module_sc16is7x0.cpp
// Description  : Class for accessing gnublin sc16is740/750/760 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sun Aug 31 16:08:28 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 859
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

#include "module_sc16is7x0.h"

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is7x0::gnublin_module_sc16is7x0(void) {

    _errorFlag = false;
    setAddress(0x20);
    setDevicefile("/dev/i2c-1");

    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address and the default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is7x0::gnublin_module_sc16is7x0(int address) {

    _errorFlag = false;
    setAddress(address);
    setDevicefile("/dev/i2c-1");

    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address to 0x20 and the given i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is7x0::gnublin_module_sc16is7x0(int address, std::string filename) {

    _errorFlag = false;
    setAddress(address);
    setDevicefile(filename);

    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
}


/**
 * @~english
 * @brief Initialize the sc16is7x0 chipset. FIFO is enabled.
 */
int gnublin_module_sc16is7x0::init(void) {

    softReset();

    initUART(CONF_INT_DFLT);

    /* Enable the transmit and receive FIFO. */
    enableFifo(1);

    return 1;
}


/**
 * @~english
 * @brief Get the last error message.
 *
 * @return The error message as c-string.
 */
const char* gnublin_module_sc16is7x0::getErrorMessage(void) {

    return _errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_sc16is7x0::fail(void) {

    return _errorFlag;
}


/**
 * @~english
 * @brief Set the i2c address.
 *
 * @param address The address to set.
 */
void gnublin_module_sc16is7x0::setAddress(int address) {

    _i2c.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file.
 *
 * @param filename The i2c device filename.
 */
void gnublin_module_sc16is7x0::setDevicefile(std::string filename) {

    _i2c.setDevicefile(filename);
}


/**
 * @~english
 * @brief Initialize the UART.
 */
int gnublin_module_sc16is7x0::initUART(unsigned char value) {

    _errorFlag = false;
    unsigned char txBuf[1];

    /* Set the MCR (Modem Control Register).
     * MCR[0] : DTR                -> 0
     * MCR[1] : RTS                -> 0
     * MCR[2] : TCR and TLR enable -> 0
     * MCR[3] : Reserved
     * MCR[4] : Enable loopback    -> 0
     * MCR[5] : Xon Any            -> 0
     * MCR[6] : IrDA mode disable  -> 0
     * MCR[7] : Clock divisor = 1  -> 0
     */
    txBuf[0] = 0x00;
    if (_i2c.send(MCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }

    /* Set the LCR (Line Control Register).
     * LCR[0] : 8 bits word            -> 1
     * LCR[1] : 8 bits word            -> 1
     * LCR[2] : 1 stop bit             -> 0
     * LCR[3] : No parity              -> 0
     * LCR[4] : No parity              -> 0
     * LCR[5] : No parity              -> 0
     * LCR[6] : No TX break condition  -> 0
     * LCR[7] : Divisor latch disabled -> 0
     */
    txBuf[0] = UART_8N1;
    if (_i2c.send(LCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    /* Set the IER (Interrupt Enable Register).
    * IER[0] : CTS interrupt enable                -> 0
    * IER[1] : RTS interrupt enable                -> 0
    * IER[2] : Xoff interrupt                      -> 0
    * IER[3] : Sleep mode                          -> 0
    * IER[4] : Modem Status interrupt              -> 0
    * IER[5] : Receive line status interrupt       -> 0
    * IER[6] : Transmit holding register interrupt -> 0
    * IER[7] : Receive holding register interrupt  -> 0
    */
    /* Set EFR[4] because IER[7:4] could only be written when EFR[4] is set.
     * EFR can only be accessed when LCR is 0xBF. */
    unsigned char lcrBuf[1];
    unsigned char efrBuf[1];

    if (_i2c.receive(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txBuf[0] = 0xbf;
    if (_i2c.send(LCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }
    
    if (_i2c.receive(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txBuf[0] = efrBuf[0] | (1 << 4);

    if (_i2c.send(EFR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the UART interrupts for the 4 MSB. */
    txBuf[0] = (CONF_INT_DFLT | value) & 0xf0;
    if (_i2c.send(IER, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (IER) Error\n";
        return -1;
    }

    /* Restore the EFR register. */
    if (_i2c.send(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (_i2c.send(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    /* Set the UART interrupts for the 4 LSB. */
    txBuf[0] = (CONF_INT_DFLT | value) & 0x0f;
    if (_i2c.send(IER, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (IER) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Reset the device.
 */
int gnublin_module_sc16is7x0::softReset(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];

    if (_i2c.receive(IOCTRL, rxBuf, 1) > 0) {
        
        txBuf[0] = rxBuf[0] | (1 << 3);
        if (_i2c.send(IOCTRL, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (IOCTRL) Error\n";
            return -1;
        }
    }
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IOCTRL) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the UART speed.
 *
 * @param baud The speed to set.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::setBaudRate(unsigned int baud) {

    _errorFlag = false;

    if ((baud < UART_300) || (baud > UART_230400)) {
        _errorFlag = true;
        sprintf(const_cast<char*>(_errorMessage.c_str()), "Baud rate is not between %d and %d\n", UART_300, UART_230400);
        return -1;
    }

    unsigned int divisor = XTAL_FREQ / (baud * 16);
    unsigned char divisorLSB = divisor;
    unsigned char divisorMSB = divisor >> 8;

    /* Set the LCR[7] to access the DLL and DLH register. */
    unsigned char txBuf[1];
    unsigned char lcrBuf[1];
    if (_i2c.receive(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txBuf[0] = lcrBuf[0] | (1 << 7);
    if (_i2c.send(LCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set the baud rate. */
    if (_i2c.send(DLL, &divisorLSB, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (DLL) Error\n";
        return -1;
    }
    if (_i2c.send(DLH, &divisorMSB, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (DLH) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (_i2c.send(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the data communication format. Word length, stop bit and parity.
 *
 * @param baud The speed to set.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::setDataFormat(unsigned char format) {

    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];

    if (_i2c.receive(LCR, rxBuf, 1) > 0) {

        txBuf[0] = rxBuf[0] | format;
        if (_i2c.send(LCR, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (LCR) Error\n";
            return -1;
        }
    }

    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Write a byte to the UART.
 *
 * @param byte The byte to write
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::writeByte(const char byte) {

    _errorFlag = false;
    unsigned char lsrBuf[1];

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    while (true) {
        if (_i2c.receive(LSR, lsrBuf, 1) < 0) {
            _errorFlag = true;
            _errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrBuf[0] & (1 << 5)) != 0) {
            /* THR empty. */
            break;
        }
    }

    if (_i2c.send(THR, (unsigned char *)(&byte), 1) > 0) {
        return 1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Write data to the UART. Blocking when no free space in TX FIFO.
 *
 * @param buffer The data to write.
 * @param len The number of bytes to write.
 * @return -1 on error or the number of bytes written on success.
 */
int gnublin_module_sc16is7x0::write(const char *buffer, unsigned int len) {

    _errorFlag = false;
    unsigned char lsrBuf[1];
    int available = 0;
    int len2send = len;
    int writeLen = 0;
    int writeBytes = 0;

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    while (true) {
        if (_i2c.receive(LSR, lsrBuf, 1) < 0) {
            _errorFlag = true;
            _errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrBuf[0] & (1 << 5)) != 0) {
            /* THR empty. */
            break;
        }

        usleep(10);
    }
    
    while (len2send > 0) {

        while ((available = (int)txAvailableSpace()) == 0 ) {
            /* Wait for space in TX FIFO */
            usleep(10);
        }

        if (len2send > available) {
            writeLen = available;
        }
        else {
            writeLen = len2send;
        }

        if (_i2c.send(THR, (unsigned char *)buffer, writeLen) < 0) {
            _errorFlag = true;
            _errorMessage = "i2c.send (THR) Error\n";
            return -1;
        }

        len2send -= writeLen;
        buffer += writeLen;
        writeBytes += writeLen;
    }

    return writeBytes;
}


/**
 * @~english
 * @brief Read a byte from the UART.
 *
 * @param byte The byte read.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::readByte(char *byte) {

    _errorFlag = false;
    int available = rxAvailableData();

    if (available < 0) {
        byte = NULL;
        return -1;
    }

    if (available == 0) {
        byte = NULL;
        return 1;
    }

    if (_i2c.receive(RHR, (unsigned char *)byte, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (RHR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Read data from the UART.
 *
 * @param buffer The data read.
 * @param len The number of bytes to read.
 * @return -1 on error and the number of bytes read on success.
 */
int gnublin_module_sc16is7x0::read(char *buffer, unsigned int len) {

    _errorFlag = false;
    unsigned int available = rxAvailableData();
    int readBytes = 0;
    int len2read = len;

    if (len2read > available) {
        len2read = available;
    }

    if (_i2c.receive(RHR, (unsigned char *)buffer, len2read) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (RHR) Error\n";
        return -1;
    }

    readBytes += len2read;

    return readBytes;
}


/**
 * @~english
 * @brief Return the number of bytes available in the RX buffer.
 *
 * @return The available data or -1 on error.
 */
int gnublin_module_sc16is7x0::rxAvailableData(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (_i2c.receive(RXLVL, rxBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (RXLVL) Error\n";
        return -1;
    }

    return rxBuf[0];
}


/**
 * @~english
 * @brief Return the number of spaces available in the TX buffer.
 *
 * @return The available space or -1 on error.
 */
int gnublin_module_sc16is7x0::txAvailableSpace(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (_i2c.receive(TXLVL, rxBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (TXLVL) Error\n";
        return -1;
    }

    return rxBuf[0];
}


/**
 * @~english
 * @brief Enable the transmit and receive FIFO.
 *
 * @param value 1 to enable and 0 to disable.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::enableFifo(int value) {

    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];

    txBuf[0] = 1 << 0;

    if (_i2c.receive(FCR, rxBuf, 1) > 0) {

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

        if (_i2c.send(FCR, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Set the trigger level of the receiver FIFO used for interrupt
 * generation.
 *
 * @param level The level to set (4 to 60).
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::rxFifoSetTriggerLevel(unsigned int level) {
    
    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];
    unsigned char lcrBuf[1];
    unsigned char efrBuf[1];

    if ((level < 4) && (level > 60)) {
        _errorFlag = true;
        _errorMessage = "Level is not between 4 and 60\n";
        return -1;
    }

    if (level % 4 != 0) {
        _errorFlag = true;
        _errorMessage = "Level is not a multiple of 4\n";
        return -1;
    }

    /* TLR can only be written to when MCR[2] and EFR[4] are set.
     * EFR can only by accessed when LCR is 0xBF. */

    /* Set MCR[2]. */
    if (_i2c.receive(MCR, rxBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txBuf[0] = rxBuf[0] | (1 << 2);

    if (_i2c.send(MCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    if (_i2c.receive(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txBuf[0] = 0xbf;
    if (_i2c.send(LCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    if (_i2c.receive(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txBuf[0] = efrBuf[0] | (1 << 4);

    if (_i2c.send(EFR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the level. */
    txBuf[0] = (level / 4) << 4;

    if (_i2c.send(TLR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    /* Restore the EFR register. */
    if (_i2c.send(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (_i2c.send(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the trigger level of the transmit FIFO used for interrupt
 * generation.
 *
 * @param level The level to set (4 to 60).
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::txFifoSetTriggerLevel(unsigned int level) {
    
    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];
    unsigned char lcrBuf[1];
    unsigned char efrBuf[1];

    if ((level < 4) && (level > 60)) {
        _errorFlag = true;
        _errorMessage = "Level is not between 4 and 60\n";
        return -1;
    }

    if (level % 4 != 0) {
        _errorFlag = true;
        _errorMessage = "Level is not a multiple of 4\n";
        return -1;
    }

    /* TCR can only be written to when MCR[2] and EFR[4] are set.
     * EFR can only by accessed when LCR is 0xBF. */

    /* Set MCR[2]. */
    if (_i2c.receive(MCR, rxBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txBuf[0] = rxBuf[0] | (1 << 2);

    if (_i2c.send(MCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    if (_i2c.receive(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txBuf[0] = 0xbf;
    if (_i2c.send(LCR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    if (_i2c.receive(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txBuf[0] = efrBuf[0] | (1 << 4);

    if (_i2c.send(EFR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the level. */
    txBuf[0] = level / 4;

    if (_i2c.send(TLR, txBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }
    
    /* Restore the EFR register. */
    if (_i2c.send(EFR, efrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (_i2c.send(LCR, lcrBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Empty the receive FIFO.
 *
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::rxEmptyFifo(void){

    _errorFlag = false;
    int available = rxAvailableData();
    unsigned char rxBuf[available];

    if (_i2c.receive(RHR, rxBuf, available) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (RHR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Reset the RX FIFO. It clear the content of the RX FIFO and resets
 * the FIFO level logic.
 *
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::resetRxFifo(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];

    if (_i2c.receive(FCR, rxBuf, 1) > 0) {

        txBuf[0] = rxBuf[0] | (1 << 1);

        if (_i2c.send(FCR, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Reset the TX FIFO. It clear the content of the TX FIFO and resets
 * the FIFO level logic.
 *
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::resetTxFifo(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];
    unsigned char txBuf[1];

    if (_i2c.receive(FCR, rxBuf, 1) > 0) {

        txBuf[0] = rxBuf[0] | (1 << 2);

        if (_i2c.send(FCR, txBuf, 1) > 0) {
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Check if an interrupt is pending or not.
 *
 * @return 1 if an interrupt is pending, 0 if not and -1 on error.
 */
int gnublin_module_sc16is7x0::isIntPending(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (_i2c.receive(IIR, rxBuf, 1) < 0) {

        if (rxBuf[0] & 0x01) {
            /* No interrupt pending. */
            return 0;
        }
        else if (rxBuf[0] & 0x00) {
            /* Interrupt pending. */
            return 1;
        }
        else {
            _errorFlag = true;
            _errorMessage = "Unknown error\n";
            return -1;
        }
    }
    
    else {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IIR) Error\n";
        return -1;
    }

    _errorFlag = true;
    _errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Identifies the pending interrupt.
 *
 * @return The pending interrupt, 0 when no interrupt pending or -1 on error.
 */
int gnublin_module_sc16is7x0::whichInt(void) {

    _errorFlag = false;
    unsigned char rxBuf[1];

    if (_i2c.receive(IIR, rxBuf, 1) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (IIR) Error\n";
        return -1;
    }

    if ((rxBuf[0] & 0x01) == 1) {
        /* No pending interrupt. */
        return 0;
    }

    //printf("rxBuf=0x%02x, 0x%02x\n", rxBuf[0], rxBuf[0] & 0x3e);
    return (rxBuf[0] & 0x3e);
}


/**
 * @~english
 * @brief Poll for an interrupt. It call the appropriate ISR callbacks when
 * an interrupt occurs.
 *
 * @return The number of interrupts or -1 on error.
 */
int gnublin_module_sc16is7x0::pollInt(void) {

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
 * @brief Register an Interrupt Service Routine that will be called when
 * data is received. It is called when the RX trigger level is reached.
 *
 * @param isr Callback function that will be called on interrupt. This is the
 * responsibility of the ISR to freed the buffer allocated memory.
 *
 * isr(char *, int)
 */
int gnublin_module_sc16is7x0::intIsrDataReceived(void (*isr)(char *, int)) {

    _isrDataReceived = isr;
    return 1;
}


/**
 * @~english
 * @brief Register an Interrupt Service Routine that will be called when
 * space is available. It is called when the TX trigger level is reached.
 *
 * @param ist Callback function that will be called on interrupt.
 *
 * isr(int)
 */
int gnublin_module_sc16is7x0::intIsrSpaceAvailable(void (*isr)(int)) {

    _isrSpaceAvailable = isr;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_sc16is7x0.cpp ends here
