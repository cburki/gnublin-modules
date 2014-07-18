// module_sc16is750.cpp --- 
// 
// Filename     : module_sc16is750.cpp
// Description  : Class for accessing gnublin sc16is750 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sun Jun 22 11:44:54 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 830
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

#define RHR     (0x00 << 3)  /* Receive Holding Register (read) */
#define THR     (0x00 << 3)  /* Transmit Holding Register (write) */
#define IER     (0x01 << 3)  /* Interupt Enable Register (read/write) */
#define IIR     (0x02 << 3)  /* Interrupt Identification Register (read) */
#define FCR     (0x02 << 3)  /* FIFO Control Register (write) */
#define LCR     (0x03 << 3)  /* Line Control Register (read/write) */
#define MCR     (0x04 << 3)  /* Modem Control Register (read/write) */
#define LSR     (0x05 << 3)  /* Line Status Register (read) */
#define MSR     (0x06 << 3)  /* Modem Status Register (read) */
#define SPR     (0x07 << 3)  /* Scratchpad Register (read/write) */
#define TCR     (0x06 << 3)  /* Transmission Control Register (read/write) - Accessible only when MCR[2] is 1 and EFR[4] is 1 */
#define TLR     (0x07 << 3)  /* Trigger Level Register (read/write) - Accessible only when MCR[2] is 1 and EFR[4] is 1 */
#define TXLVL   (0x08 << 3)  /* Transmit FIFO Level Register (read) */
#define RXLVL   (0x09 << 3)  /* Receive FIFO Level Register (read) */
#define IODIR   (0x0A << 3)  /* IO Pin Direction Register (read/write) */
#define IOSTATE (0x0B << 3)  /* IO States Register (read) */
#define IOINTEN (0x0C << 3)  /* IO Interrupt Enable Register (read/write) */
#define IOCTRL  (0x0E << 3)  /* IO Control Register (read/write) */
#define EFCRF   (0x0F << 3)  /* Extra Features Register (read/write) */
#define DLL     (0x00 << 3)  /* Divisor Latch LSB (read/write) - Accessile only when LCR[7] is logic 1 */
#define DLH     (0x01 << 3)  /* Divisor Latch MSB (read/write) - Accessile only when LCR[7] is logic 1 */
#define EFR     (0x02 << 3)  /* Enhanced Features Register (read/write) - Accessible only when LCR is set to 10111111 (0xBF) */
#define XON1    (0x04 << 3)  /* Xon1 Word (read/write) - Accessible only when LCR is set to 10111111 (0xBF) */
#define XON2    (0x05 << 3)  /* Xon2 Word (read/write) - Accessible only when LCR is set to 10111111 (0xBF) */
#define XOFF1   (0x06 << 3)  /* Xoff1 Word (read/write) - Accessible only when LCR is set to 10111111 (0xBF) */
#define XOFF2   (0x07 << 3)  /* Xoff2 Word (read/write) - Accessible only when LCR is set to 10111111 (0xBF) */

/* Registers access are shifter 3 bit left because they are encoded in the bits 3:0 of
   the subaddress. (See section 10.4 of the SC16IS750 datasheet) */

#define XTAL_FREQ 14745600

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(void) {

    _errorFlag = false;
    setAddress(0x20);
    setDevicefile("/dev/i2c-1");

    _ioLatchReg = 0x00;
    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address and the default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(int address) {

    _errorFlag = false;
    setAddress(address);
    setDevicefile("/dev/i2c-1");

    _ioLatchReg = 0x00;
    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Set the given i2c address to 0x20 and the given i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is750::gnublin_module_sc16is750(int address, std::string filename) {

    _errorFlag = false;
    setAddress(address);
    setDevicefile(filename);

    _ioLatchReg = 0x00;
    _isrDataReceived = NULL;
    _isrSpaceAvailable = NULL;
    _isrIO = NULL;
}


/**
 * @~english
 * @brief Initialize the sc16is750 chipset. FIFO is enabled.
 */
int gnublin_module_sc16is750::init(void) {

    softReset();

    initUART(CONF_INT_DFLT);
    initIO(CONF_IO_DFLT);

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
const char* gnublin_module_sc16is750::getErrorMessage(void) {

    return _errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_sc16is750::fail(void) {

    return _errorFlag;
}


/**
 * @~english
 * @brief Set the i2c address.
 *
 * @param address The address to set.
 */
void gnublin_module_sc16is750::setAddress(int address) {

    _i2c.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file.
 *
 * @param filename The i2c device filename.
 */
void gnublin_module_sc16is750::setDevicefile(std::string filename) {

    _i2c.setDevicefile(filename);
}


/**
 * @~english
 * @brief Initialize the UART.
 */
int gnublin_module_sc16is750::initUART(unsigned char value) {

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
int gnublin_module_sc16is750::softReset(void) {

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
int gnublin_module_sc16is750::setBaudRate(unsigned int baud) {

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
int gnublin_module_sc16is750::setDataFormat(unsigned char format) {

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
int gnublin_module_sc16is750::writeByte(const char byte) {

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
int gnublin_module_sc16is750::write(const char *buffer, unsigned int len) {

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
int gnublin_module_sc16is750::readByte(char *byte) {

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
int gnublin_module_sc16is750::read(char *buffer, unsigned int len) {

    _errorFlag = false;
    unsigned int available = rxAvailableData();
    int readLen = len;

    if (len > available) {
        readLen = available;
    }

    if (_i2c.receive(RHR, (unsigned char *)buffer, readLen) < 0) {
        _errorFlag = true;
        _errorMessage = "i2c.receive (RHR) Error\n";
        return -1;
    }

    return readLen;
}


/**
 * @~english
 * @brief Return the number of bytes available in the RX buffer.
 *
 * @return The available data or -1 on error.
 */
int gnublin_module_sc16is750::rxAvailableData(void) {

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
int gnublin_module_sc16is750::txAvailableSpace(void) {

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
int gnublin_module_sc16is750::enableFifo(int value) {

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
int gnublin_module_sc16is750::rxFifoSetTriggerLevel(unsigned int level) {
    
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
int gnublin_module_sc16is750::txFifoSetTriggerLevel(unsigned int level) {
    
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
int gnublin_module_sc16is750::rxEmptyFifo(void){

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
int gnublin_module_sc16is750::resetRxFifo(void) {

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
int gnublin_module_sc16is750::resetTxFifo(void) {

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
 * @brief Check if an interrupt is pending or not.
 *
 * @return 1 if an interrupt is pending, 0 if not and -1 on error.
 */
int gnublin_module_sc16is750::isIntPending(void) {

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
int gnublin_module_sc16is750::whichInt(void) {

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
 * @brief Register an Interrupt Service Routine that will be called when
 * data is received. It is called when the RX trigger level is reached.
 *
 * @param isr Callback function that will be called on interrupt. This is the
 * responsibility of the ISR to freed the buffer allocated memory.
 *
 * isr(char *, int)
 */
int gnublin_module_sc16is750::intIsrDataReceived(void (*isr)(char *, int)) {

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
int gnublin_module_sc16is750::intIsrSpaceAvailable(void (*isr)(int)) {

    _isrSpaceAvailable = isr;
    return 1;
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
