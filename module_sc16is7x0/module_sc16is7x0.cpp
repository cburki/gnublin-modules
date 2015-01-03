// module_sc16is7x0.cpp --- 
// 
// Filename     : module_sc16is7x0.cpp
// Description  : Class for accessing gnublin sc16is740/750/760 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sat Jan  3 09:23:15 2015 (3600 CET)
//           By : Christophe Burki
//     Update # : 1426
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

#define USE_ENHANCED_FIFO (1)  /* Whether to use TLR (enhanced) or FCR. */

/* UART non enhanced (FCR) FIFO values. */
#define UART_RX_FIFO_CLEAR 0x3f
#define UART_RX_FIFO_8     0x00
#define UART_RX_FIFO_16    0x40
#define UART_RX_FIFO_56    0x80
#define UART_RX_FIFO_60    0xc0
#define UART_TX_FIFO_CLEAR 0xcf
#define UART_TX_FIFO_8     0x00
#define UART_TX_FIFO_16    0x10
#define UART_TX_FIFO_32    0x20
#define UART_TX_FIFO_56    0x30

/* Default resume and halt triggers. */
#define TRIGGER_DEFAULT_RESUME  48
#define TRIGGER_DEFAULT_HALT    24

/* Default RX and TX fifo level. */
#define RX_DEFAULT_LEVEL        1
#define TX_DEFAULT_LEVEL        1

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

    config.fifoEnable = 0;
    config.fcrRegister = 0x00;
    
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
 * @brief Initialize the sc16is7x0 chipset.
 */
int gnublin_module_sc16is7x0::init(void) {

    softReset();
    usleep(10 * 1000);
    initUART();

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
 * 9600 baud, 8N1, CTS and RTS flow control disabled, FIFO disabled.
 * Disable all interrupts.
 */
int gnublin_module_sc16is7x0::initUART(void) {

    errorFlag = false;

    if (setBaudRate(UART_9600) < 0) {
        errorFlag = true;
        return -1;
    }
    
    if (setDataFormat(UART_8N1) < 0) {
        errorFlag = true;
        return -1;
    }

    if (resetRxFifo() < 0) {
        errorFlag = true;
        return -1;
    }
    
    if (resetTxFifo() < 0) {
        errorFlag = true;
        return -1;
    }

    if (enableFifo(0) < 0) {
        errorFlag = true;
        return -1;
    }

    if (setFlowControl(CONF_FLOW_DISABLED) < 0) {
    	errorFlag = true;
        return -1;
    }
    
#if (USE_ENHANCED_FIFO)
    if (setFlowTriggers(TRIGGER_DEFAULT_RESUME, TRIGGER_DEFAULT_HALT) < 0) {
    	errorFlag = true;
        return -1;
    }
#endif

    if (setModemControl() < 0) {
    	errorFlag = true;
        return -1;
    }
    
    if (setInterrupt(CONF_INT_DISABLED) < 0) {
        errorFlag = true;
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

    if (i2c.receive(IOCTRL, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (IOCTRL) Error\n";
        return -1;
    }
        
    txValue = rxValue | (1 << 3);
    if (i2c.send(IOCTRL, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (IOCTRL) Error\n";
        return -1;
    }

    return 1;
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
        errorMessage = "i2c.send (LCR) Error\n";
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

    if (i2c.receive(LCR, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    txValue = format;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the modem control.
 * Enable TCR and TLR registers.
 */
int gnublin_module_sc16is7x0::setModemControl(void) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char lcrValue;
    unsigned char efrValue;

    /* MCR[2] can only be accessed when EFR[4] is set.
       EFR can only be accessed when LCR is 0xBF. */

    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }
    
    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    txValue = 0x00;
#if (USE_ENHANCED_FIFO)
    /* Set MCR[2] to enable TCR and TLR. */
    txValue |= (1 << 2);
#endif
    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
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
 * @brief Set the flow control according to the given mask.
 * Software flow control is not yet supported.
 *
 * @param flow The bit mask to set the flow control.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::setFlowControl(unsigned char flow) {

    //printf("setFlowControl(flow=0x%02x)\n", flow);

    errorFlag = false;
    unsigned char txValue;
    unsigned char lcrValue;

    /* EFR can only be accessed when LCR is 0xBF. */

    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    /* Set EFR with given flow mask. */
    txValue = flow;
    if (i2c.send(EFR, &txValue, 1) < 0) {
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
 * @brief Set the flow control levels for resuming and halting the transmission.
 *
 * @param resume Trigger level to resume transmission (0 to 60 with a granularity of 4).
 * @param halt Trigger level to halt transmission (0 to 60 with a granularity of 4).
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::setFlowTriggers(unsigned int resume, unsigned int halt) {

    //printf("setFlowTriggers()\n");

#if !(USE_ENHANCED_FIFO)
    printf("Flow triggers could only be set in enhanced mode.\n");
    return 1;
#endif
    
    errorFlag = false;
    unsigned char txValue;
    unsigned char lcrValue;
    unsigned char efrValue;

    /* Sanity checks for resume and halt values. */
    if ((resume < 0) && (resume > 60)) {
        errorFlag = true;
        errorMessage = "Resume is not between 4 and 60\n";
        return -1;
    }

    if (resume % 4 != 0) {
        errorFlag = true;
        errorMessage = "Resume is not a multiple of 4\n";
        return -1;
    }

    if ((halt < 0) && (halt > 60)) {
        errorFlag = true;
        errorMessage = "Halt is not between 4 and 60\n";
        return -1;
    }

    if (halt % 4 != 0) {
        errorFlag = true;
        errorMessage = "Halt is not a multiple of 4\n";
        return -1;
    }

    if (halt > resume) {
       	errorFlag = true;
        errorMessage = "Halt can not be lower than resume\n";
        return -1;
    }
    
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* TCR can only be accessed when EFR[4] and MCR[2] are set. EFR can only be
       accessed when LCR is 0xBF.
       MCR[2] has already been set in setModemControl. */

    /* Set LCR to 0xBF to enable access to EFR register. */
    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    
    /* Set TCR.
     * TCR[0-3] : RX Fifo trigger level to halt transmission.
     * TCR[4-7] : RX Fifo trigger level to resume.
     */
    txValue = ((resume / 4) << 4) | (halt / 4);
    if (i2c.send(TCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TCR) Error\n";
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
 * @brief Set the interrupt to enable.
 *
 * @param interrupt Interrupt mask to set to enable interrupts.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::setInterrupt(unsigned char interrupt) {

    errorFlag = false;
    unsigned char txValue;
    unsigned char ierValue;
    unsigned char lcrValue;
    unsigned char efrValue;

    /* IER can only be accessed when EFR[4] is set.
       EFR can only be accessed when LCR is 0xBF.
       IER[7:4] can only be accessed when EFR[4] is set. */
    
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }
 
    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

    /* Set the UART interrupts for the 4 MSB. */
    ierValue = (CONF_INT_NONE | interrupt) & 0xf0;
    if (i2c.send(IER, &ierValue, 1) < 0) {
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
    ierValue = ierValue | ((CONF_INT_NONE | interrupt) & 0x0f);
    if (i2c.send(IER, &ierValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (IER) Error\n";
        return -1;
    }

    return 1;
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
#if !(USE_ENHANCED_FIFO)
    unsigned char txValue;
    unsigned char lcrValue;
    unsigned char efrValue;
#endif

    /* FCR[4:5] can only be accessed when EFR[4] is set.
       EFR can only be accessed when LCR is 0xBF. */

#if !(USE_ENHANCED_FIFO)
    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
    txValue = 0xbf;
    if (i2c.send(LCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (LCR) Error\n";
        return -1;
    }

    if (i2c.receive(EFR, &efrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (EFR) Error\n";
        return -1;
    }

    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }
#endif
    
    /* Set FCR. */
#if !(USE_ENHANCED_FIFO)
    if (value == 1) {
        config.fcrRegister |= (1 << 0);
    }
    else {
        config.fcrRegister |= (config.fcrRegister & ~(1 << 0));
    }
#else
    if (value == 1) {
        config.fcrRegister = 0x01;
    }
    else {
        config.fcrRegister = 0x00;
    }
#endif
    if (i2c.send(FCR, &config.fcrRegister, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }
    
#if !(USE_ENHANCED_FIFO)
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
#endif
    
    config.fifoEnable = value;
    return 1;
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
    
    //printf("rxFifoSetTriggerLevel(level=%d)\n", level);

    errorFlag = false;
    unsigned char txValue;
    unsigned char lcrValue;
    unsigned char efrValue;
#if (USE_ENHANCED_FIFO)
    unsigned char tlrValue;
#else
    unsigned char mcrValue;
#endif
    
    if (config.fifoEnable == 0) {
        return 1;
    }

#if (USE_ENHANCED_FIFO)
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
#else

    if ((level != 8) && (level != 16) && (level != 56) && (level != 60)) {
        errorFlag = true;
        errorMessage = "Level is not 8, 16, 56 or 60\n";
        return -1;
    }
#endif

    /* TLR can only be accessed when MCR[2] and EFR[4] are set.
       FCR[5:4] (TX trigger) can only be accessed when EFR[4] is set.
       FCR[7:6] (RX trigger) can be accessed without restrictions.
       EFR can only by accessed when LCR is 0xBF. */

    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
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

    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

#if (USE_ENHANCED_FIFO)
    /* Set the level. */
    if (i2c.receive(TLR, &tlrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (TLR) Error\n";
        return -1;
    }

    txValue = (tlrValue & 0x0f) | ((level / 4) << 4);
    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    config.fcrRegister &= (UART_RX_FIFO_CLEAR & UART_TX_FIFO_CLEAR);
    if (i2c.send(FCR, &config.fcrRegister, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }
    
#else
    config.fcrRegister &= UART_RX_FIFO_CLEAR;
    switch (level) {
    case 8 :
        config.fcrRegister |= UART_RX_FIFO_8;
        break;
    case 16 :
        config.fcrRegister |= UART_RX_FIFO_16;
        break;
    case 56 :
        config.fcrRegister |= UART_RX_FIFO_56;
        break;
    case 60 :
        config.fcrRegister |= UART_RX_FIFO_60;
        break;
    default :
        config.fcrRegister |= UART_RX_FIFO_8;
        break;
    }
    if (i2c.send(FCR, &config.fcrRegister, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }

    if (i2c.receive(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set MCR[2]. */
    txValue = mcrValue | (1 << 2);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }

    /* Reset the TLR. */
    txValue = 0x00;
    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    /* Restore the MCR register. */
    if (i2c.send(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }
#endif

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
    
    //printf("txFifoSetTriggerLevel(level=%d)\n", level);

    errorFlag = false;
    unsigned char txValue;
    unsigned char lcrValue;
    unsigned char efrValue;
#if (USE_ENHANCED_FIFO)
    unsigned char tlrValue;
#else
    unsigned char mcrValue;
#endif

    if (config.fifoEnable == 0) {
        return 1;
    }

#if (USE_ENHANCED_FIFO)
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
#else
    
    if ((level != 8) && (level != 16) && (level != 32) && (level != 56)) {
        errorFlag = true;
        errorMessage = "Level is not 8, 16, 32 or 56\n";
        return -1;
    }
#endif

    /* TLR can only be accessed when MCR[2] and EFR[4] are set.
       FCR[5:4] (TX trigger) can only be accessed when EFR[4] is set.
       FCR[7:6] (RX trigger) can be accessed without restrictions.
       EFR can only by accessed when LCR is 0xBF. */

    if (i2c.receive(LCR, &lcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LCR) Error\n";
        return -1;
    }

    /* Set LCR to 0xBF to enable access to EFR register. */
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

    /* Set EFR[4]. */
    txValue = efrValue | (1 << 4);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (EFR) Error\n";
        return -1;
    }

#if (USE_ENHANCED_FIFO)
    /* Set the level. */
    if (i2c.receive(TLR, &tlrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (TLR) Error\n";
        return -1;
    }

    txValue = (tlrValue & 0xf0) | (level / 4);
    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    config.fcrRegister &= (UART_RX_FIFO_CLEAR & UART_TX_FIFO_CLEAR);
    if (i2c.send(FCR, &config.fcrRegister, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }

#else
    config.fcrRegister &= UART_TX_FIFO_CLEAR;
    switch (level) {
    case 8 :
        config.fcrRegister |= UART_TX_FIFO_8;
        break;
    case 16 :
        config.fcrRegister |= UART_TX_FIFO_16;
        break;
    case 56 :
        config.fcrRegister |= UART_TX_FIFO_32;
        break;
    case 60 :
        config.fcrRegister |= UART_TX_FIFO_56;
        break;
    default :
        config.fcrRegister |= UART_TX_FIFO_8;
        break;
    }
    if (i2c.send(FCR, &config.fcrRegister, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }

    if (i2c.receive(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    /* Set MCR[2]. */
    txValue = mcrValue | (1 << 2);
    if (i2c.send(EFR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }

    /* Reset the TLR. */
    txValue = 0x00;
    if (i2c.send(TLR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (TLR) Error\n";
        return -1;
    }

    /* Restore the MCR register. */
    if (i2c.send(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }
#endif
    
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

    if (available == 0) {
        return 1;
    }

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
    unsigned char txValue;

    txValue = config.fcrRegister | (1 << 1);
    if (i2c.send(FCR, &txValue, 1) > 0) {
        usleep(10);
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
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
    unsigned char txValue;

    txValue = config.fcrRegister | (1 << 2);
    if (i2c.send(FCR, &txValue, 1) > 0) {
        usleep(10);
        return 1;
    }
    else {
        errorFlag = true;
        errorMessage = "i2c.send (FCR) Error\n";
        return -1;
    }

    errorFlag = true;
    errorMessage = "Unknown Error\n";
    return -1;
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
 * @brief Read the receiver and transmitter status (LSR Line Status Register).
 *
 * @return The status read from the Line Status Register or -1 on error.
 */
unsigned char gnublin_module_sc16is7x0::readLineStatus(void) {

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(LSR, &rxValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (LSR) Error\n";
        return -1;
    }

    return rxValue;
}


/**
 * @~english
 * @brief Enable the loopback mode (for test purpose only).
 * The MCR[1:0] signals are looped back int MSR[4:5] and the TX output
 * is looped back to the RX input internally.
 *
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sc16is7x0::enableLoopback(void) {

    errorFlag = false;
    unsigned char mcrValue;
    unsigned char txValue = 0;

    if (i2c.receive(MCR, &mcrValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.receive (MCR) Error\n";
        return -1;
    }

    txValue = mcrValue | 0x10;
    if (i2c.send(MCR, &txValue, 1) < 0) {
        errorFlag = true;
        errorMessage = "i2c.send (MCR) Error\n";
        return -1;
    }

    return 1;
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
    //unsigned char lsrValue;

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    /*
    while (true) {
        if (i2c.receive(LSR, &lsrValue, 1) < 0) {
            errorFlag = true;
            errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrValue & (1 << 5)) != 0) {
    */
            /* THR empty. */
    /*
            break;
        }
    }
    */
    /* Wait for space in TX buffer. */
    while (txAvailableSpace() <= 0) {
        	usleep(10);
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
    //unsigned char lsrValue;
    int available = 0;
    int len2send = len;
    int writeLen = 0;
    int writeBytes = 0;

    /* Wait for the THR (Transmit Holding Register) to be empty. */
    /*
    while (true) {
        if (i2c.receive(LSR, &lsrValue, 1) < 0) {
            errorFlag = true;
            errorMessage = "i2c.receive (LSR) Error\n";
            return -1;
        }

        if ((lsrValue & (1 << 5)) != 0) {
    */
            /* THR empty. */
    /*
            break;
        }

        usleep(10);
    }
    */

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

        if (config.fifoEnable == 1) {
            if (i2c.send(THR, (unsigned char *)buffer, writeLen) < 0) {
                errorFlag = true;
                errorMessage = "i2c.send (THR) Error\n";
                return -1;
            }
        }
        else {
            int writeCount = 0;
            while (writeCount <= writeLen) {
                if (writeByte(buffer[writeCount]) < 0) {
                }
                writeCount++;
            }
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
 * @brief Check if an interrupt is pending or not.
 *
 * @return 1 if an interrupt is pending, 0 if not and -1 on error.
 */
int gnublin_module_sc16is7x0::isIntPending(void) {

    errorFlag = false;
    unsigned char rxValue;

    if (i2c.receive(IIR, &rxValue, 1) > 0) {

        if ((rxValue & 0x01) == 1) {
            /* No interrupt pending. */
            return 0;
        }
        else if ((rxValue & 0x01) == 0) {
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

    //printf("rxValue=0x%02x, 0x%02x\n", rxValue, rxValue & 0x3e);
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
    case INT_RLSE :  /* Receiver line status error. */
        //break;
    case INT_RTOUT :  /* Receiver timeout. */
        //break;
    case INT_RHR :  /* RHR. */
        if (isrDataReceived != NULL) {
            int available = rxAvailableData();
            if (available > 0) {
                char *buffer = (char *)malloc(available + 1);
                read(buffer, available);
                buffer[available] = '\0';
                
                isrDataReceived(buffer, available + 1);
            }
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
