// module_sc16is7x0.cpp --- 
// 
// Filename     : module_sc16is7x0.cpp
// Description  : Class for accessing gnublin sc16is740/750/760 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sun Dec 14 20:10:43 2014 (3600 CET)
//           By : Christophe Burki
//     Update # : 995
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
 * @brief Set the given i2c address to 0x20 and the given i2c file to /dev/i2c-1.
 *
 * @param address The i2c address.
 * @param filename The i2c device file.
 */
gnublin_module_sc16is7x0::gnublin_module_sc16is7x0(int address, std::string filename) {

    errorFlag = false;
    setAddress(address);
    setDevicefile(filename);

    isrDataReceived = NULL;
    isrSpaceAvailable = NULL;
}


/**
 * @~english
 * brief
 */
gnublin_module_sc16is7x0::~gnublin_module_sc16is7x0(void) {
}


/**
 * @~english
 * @brief Initialize the sc16is7x0 chipset. FIFO is enabled.
 */
int gnublin_module_sc16is7x0::init(void) {

    softReset();

    initUART(CONF_INT_DFLT);

    return 1;
}


/**
 * @~english
 * @brief Get the last error message.
 *
 * @return The error message as c-string.
 */
const char* gnublin_module_sc16is7x0::getErrorMessage(void) {

    return errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_sc16is7x0::fail(void) {

    return errorFlag;
}


/**
 * @~english
 * @brief Set the i2c address.
 *
 * @param address The address to set.
 */
void gnublin_module_sc16is7x0::setAddress(int address) {

    i2c.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file.
 *
 * @param filename The i2c device filename.
 */
void gnublin_module_sc16is7x0::setDevicefile(std::string filename) {

    i2c.setDevicefile(filename);
}


/**
 * @~english
 * @brief Initialize the UART.
 */
int gnublin_module_sc16is7x0::initUART(unsigned char value) {

    errorFlag = false;
    unsigned char txValue;

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
    txValue = 0x00;
    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
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
    txValue = UART_8N1;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    /* Set EFR[4].
     * Can only be written when EFR[4] is set. EFR can only be accessed when LCR is 0xBF.
     */
    unsigned char lcrValue;
    unsigned char efrValue;
    unsigned char mcrValue;

    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }
    
    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txValue = efrValue | (1 << 4);

    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set MCR[2]. */
    if (i2c.receive(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txValue = mcrValue | (1 << 2);

    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }
    
    /* Set the TCR (Transmission Control Register).
     * TCR[0-3] : RX Fifo trigger level to halt transmission -> 0x0C (48)
     * TCR[4-7] : RX Fifo trigger level to resume            -> 0x60 (24)
     *
     * Can only be written when EFR[4] is set.
     */
    txValue = 0x6c;
    if (i2c.send(TCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TCR) Error\n";
        return -1;
    }

    /* Set the IER (Interrupt Enable Register). Set the UART interrupts for the 4 MSB.
     * IER[0] : Receive holding register interrupt  -> 0
     * IER[1] : Transmit holding register interrupt -> 0
     * IER[2] : Receive line status interrupt       -> 0
     * IER[3] : Modem Status interrupt              -> 0
     * IER[4] : Sleep mode                          -> 0
     * IER[5] : Xoff interrupt                      -> 0
     * IER[6] : RTS interrupt enable                -> 0
     * IER[7] : CTS interrupt enable                -> 0
     *
     * Can only be written when EFR[4] is set.
     */

    txValue = (CONF_INT_DFLT | value) & 0xf0;
    if (i2c.send(IER, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (IER) Error\n";
        return -1;
    }

    /* Restore the EFR register. */
    if (i2c.send(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (i2c.send(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    /* Set the UART interrupts for the 4 LSB. */
    txValue = (CONF_INT_DFLT | value) & 0x0f;
    if (i2c.send(IER, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (IER) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Reset the device.
 */
int gnublin_module_sc16is7x0::softReset(void) {

    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;

    if (i2c.receive(IOCTRL, &rxValue, 1) > 0) {
        
        txValue = rxValue | (1 << 3);
        if (i2c.send(IOCTRL, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send (IOCTRL) Error\n";
            return -1;
        }
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.receive (IOCTRL) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
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

    errorFlag = false;

    if ((baud < UART_300) || (baud > UART_230400)) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Baud rate is not between %d and %d\n", UART_300, UART_230400);
        return -1;
    }

    unsigned int divisor = XTAL_FREQ / (baud * 16);
    unsigned char divisorLSB = divisor;
    unsigned char divisorMSB = divisor >> 8;

    /* Set the LCR[7] to access the DLL and DLH register. */
    unsigned char txValue;
    unsigned char lcrValue;
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txValue = lcrValue | (1 << 7);
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set the baud rate. */
    if (i2c.send(DLL, &divisorLSB, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (DLL) Error\n";
        return -1;
    }
    if (i2c.send(DLH, &divisorMSB, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (DLH) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (i2c.send(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
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

    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;

    if (i2c.receive(LCR, &rxValue, 1) > 0) {

        txValue = rxValue | format;
        if (i2c.send(LCR, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send (LCR) Error\n";
            return -1;
        }
    }

    else {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
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

    errorFlag = false;
    unsigned char lsrValue;

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    while (true) {
        if (i2c.receive(LSR, &lsrValue, 1) < 0) {
            errorFlag = true;
            errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrValue & (1 << 5)) != 0) {
            /* THR empty. */
            break;
        }
    }

    if (i2c.send(THR, (unsigned char *)(&byte), 1) > 0) {
        return 1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
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

    errorFlag = false;
    unsigned char lsrValue;
    int available = 0;
    int len2send = len;
    int writeLen = 0;
    int writeBytes = 0;

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    while (true) {
        if (i2c.receive(LSR, &lsrValue, 1) < 0) {
            errorFlag = true;
            errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrValue & (1 << 5)) != 0) {
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

        if (i2c.send(THR, (unsigned char *)buffer, writeLen) < 0) {
            errorFlag = true;
            errorMessage = "i2c.send (THR) Error\n";
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

    errorFlag = false;
    int available = rxAvailableData();

    if (available < 0) {
        byte = NULL;
        return -1;
    }

    if (available == 0) {
        byte = NULL;
        return 1;
    }

    if (i2c.receive(RHR, (unsigned char *)byte, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (RHR) Error\n";
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

    errorFlag = false;
    unsigned int available = rxAvailableData();
    int readBytes = 0;
    unsigned int len2read = len;

    if (len2read > available) {
        len2read = available;
    }

    if (i2c.receive(RHR, (unsigned char *)buffer, len2read) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (RHR) Error\n";
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

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(RXLVL, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (RXLVL) Error\n";
        return -1;
    }

    return rxValue;
}


/**
 * @~english
 * @brief Return the number of spaces available in the TX buffer.
 *
 * @return The available space or -1 on error.
 */
int gnublin_module_sc16is7x0::txAvailableSpace(void) {

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(TXLVL, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (TXLVL) Error\n";
        return -1;
    }

    return rxValue;
}


/**
 * @~english
 * @brief Enable the transmit and receive FIFO.
 *
 * @param value 1 to enable and 0 to disable.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::enableFifo(int value) {

    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;

    txValue = 1 << 0;

    if (i2c.receive(FCR, &rxValue, 1) > 0) {

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

        if (i2c.send(FCR, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        errorFlag = true;
        errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
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
    
    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;
    unsigned char tlrValue;
    unsigned char lcrValue;
    unsigned char efrValue;

    if ((level < 4) && (level > 60)) {
        errorFlag = true;
        errorMessage = "Level is not between 4 and 60\n";
        return -1;
    }

    if (level % 4 != 0) {
        errorFlag = true;
        errorMessage = "Level is not a multiple of 4\n";
        return -1;
    }

    /* TLR can only be written to when MCR[2] and EFR[4] are set.
     * EFR can only by accessed when LCR is 0xBF. */

    /* Set MCR[2]. */
    if (i2c.receive(MCR, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txValue = rxValue | (1 << 2);

    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txValue = efrValue | (1 << 4);

    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the level. */
    if (i2c.receive(TLR, &tlrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txValue = (tlrValue & 0x0f) | ((level / 4) << 4);

    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    /* Restore the EFR register. */
    if (i2c.send(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (i2c.send(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
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
    
    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;
    unsigned char tlrValue;
    unsigned char lcrValue;
    unsigned char efrValue;

    if ((level < 4) && (level > 60)) {
        errorFlag = true;
        errorMessage = "Level is not between 4 and 60\n";
        return -1;
    }

    if (level % 4 != 0) {
        errorFlag = true;
        errorMessage = "Level is not a multiple of 4\n";
        return -1;
    }

    /* TLR can only be written to when MCR[2] and EFR[4] are set.
     * EFR can only by accessed when LCR is 0xBF. */

    /* Set MCR[2]. */
    if (i2c.receive(MCR, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txValue = rxValue | (1 << 2);

    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txValue = efrValue | (1 << 4);

    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the level. */
    if (i2c.receive(TLR, &tlrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    txValue = (tlrValue & 0xf0) | (level / 4);

    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }
    
    /* Restore the EFR register. */
    if (i2c.send(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Restore the LCR register. */
    if (i2c.send(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
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

    errorFlag = false;
    int available = rxAvailableData();
    unsigned char buffer[available];

    if (i2c.receive(RHR, buffer, available) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (RHR) Error\n";
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

    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;

    if (i2c.receive(FCR, &rxValue, 1) > 0) {

        txValue = rxValue | (1 << 1);

        if (i2c.send(FCR, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        errorFlag = true;
        errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
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

    errorFlag = false;
    unsigned char rxValue;
    unsigned char txValue;

    if (i2c.receive(FCR, &rxValue, 1) > 0) {

        txValue = rxValue | (1 << 2);

        if (i2c.send(FCR, &txValue, 1) > 0) {
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "i2c.send (FCR) Error\n";
            return -1;
        }
    }

    else {
        errorFlag = true;
        errorMessage = "i2c.receive (FCR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
    return -1;
}


/**
 * @~english
 * @brief Check if an interrupt is pending or not.
 *
 * @return 1 if an interrupt is pending, 0 if not and -1 on error.
 */
int gnublin_module_sc16is7x0::isIntPending(void) {

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(IIR, &rxValue, 1) > 0) {

        if (rxValue & 0x01) {
            /* No interrupt pending. */
            return 0;
        }
        else if (rxValue & 0x00) {
            /* Interrupt pending. */
            return 1;
        }
        else {
            errorFlag = true;
            errorMessage = "Unknown error\n";
            return -1;
        }
    }
    
    else {
        errorFlag = true;
        errorMessage = "i2c.receive (IIR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown error\n";
    return -1;
}


/**
 * @~english
 * @brief Identifies the pending interrupt.
 *
 * @return The pending interrupt, 0 when no interrupt pending or -1 on error.
 */
int gnublin_module_sc16is7x0::whichInt(void) {

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(IIR, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (IIR) Error\n";
        return -1;
    }

    if ((rxValue & 0x01) == 1) {
        /* No pending interrupt. */
        return 0;
    }

    //printf("&rxValue=0x%02x, 0x%02x\n", rxValue, rxValue & 0x3e);
    return (rxValue & 0x3e);
}


/**
 * @~english
 * @brief Poll for an interrupt. It call the appropriate ISR callbacks when
 * an interrupt occurs.
 *
 * @return The number of interrupts or -1 on error.
 */
int gnublin_module_sc16is7x0::pollInt(void) {

    errorFlag = false;

    int count = 0;
    int interrupt = whichInt();

    if (interrupt == 0) {
        /* No pending interrupt. */
        return interrupt;
    }
    if (interrupt < 0) {
        /* Error while identifying interrupt. */
        return interrupt;
    }

    switch (interrupt) {
    case INT_RLS :  /* Receiver line status error. */
        //break;
    case INT_RTOUT :  /* Receiver timeout. */
        //break;
    case INT_RHR :  /* RHR. */
        if (isrDataReceived != NULL) {
            int available = rxAvailableData();
            char *buffer = (char *)malloc(available + 1);
            read(buffer, available);
            buffer[available] = '\0';
            
            isrDataReceived(buffer, available + 1);
        }
        count++;
        break;
    case INT_THR :  /* THR. */
        if (isrSpaceAvailable != NULL) {
            int available = txAvailableSpace();
            
            isrSpaceAvailable(available);
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
        errorFlag = true;
        errorMessage = "Unknown interrupt source\n";
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

    isrDataReceived = isr;
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

    isrSpaceAvailable = isr;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_sc16is7x0.cpp ends here
