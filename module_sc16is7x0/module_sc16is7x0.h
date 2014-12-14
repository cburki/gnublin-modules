/* module_sc16is7x0.h --- 
 * 
 * Filename     : module_sc16is7x0.h
 * Description  : Class for accessing gnublin SC16is740/750/760 single UART.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Thu May 29 15:08:43 2014
 * Version      : 
 * Last-Updated : Sat Dec 13 16:38:41 2014 (3600 CET)
 *           By : Christophe Burki
 *     Update # : 132
 * URL          : 
 * Keywords     : 
 * Compatibility: 
 * 
 */

/* Commentary   : 
 *
 * I2C Address 
 * 
 * A1 | A0 | I2C Address
 * ---------------------
 * 0  | 0  | 0x4d
 * 0  | 1  | 0x4c
 * 1  | 0  | 0x49
 * 1  | 1  | 0x48
 *
 * 
 * Interrupts
 *
 * RHR Interrupt : Receive data ready (FIFO disable) or RX FIFO above
 *                 trigger level (FIFO enable).
 *                 The data must be read in order to clear the pending
 *                 interrupt. If the data available ISR is registered
 *                 (via intIsrDataAvailable), all data in the RX FIFO
 *                 is read.
 * THR Interrupt : Transmit FIFO empty (FIFO disable) or TX FIFO passes
 *                 above the trigger level (FIFO enable).
 * I/O pins      : Input pins change of state.
 *
 */

/* Change log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as 
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file LICENSE.  If not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * ;; Floor, Boston, MA 02110-1301, USA.
 */

/* Code         : */

#ifndef GNUBLIN_MODULE_SC16IS7x0
#define GNUBLIN_MODULE_SC16IS7x0

/* -------------------------------------------------------------------------- */

#include "gnublin.h"

/* -------------------------------------------------------------------------- */

/* UART baud rate. */
#define UART_300       300
#define UART_600       600
#define UART_1200     1200
#define UART_2400     2400
#define UART_4800     4800
#define UART_9600     9600
#define UART_14400   14400
#define UART_19200   19200
#define UART_28800   28800
#define UART_38400   38400
#define UART_57600   57600
#define UART_115200 115200
#define UART_230400 230400

/* UART data communication format. */
#define UART_5N1 0x00
#define UART_6N1 0x01
#define UART_7N1 0x02
#define UART_8N1 0x03
#define UART_5N2 0x04
#define UART_6N2 0x05
#define UART_7N2 0x06
#define UART_8N2 0x07
#define UART_5O1 0x08
#define UART_6O1 0x09
#define UART_7O1 0x0A
#define UART_8O1 0x0B
#define UART_5O2 0x0C
#define UART_6O2 0x0D
#define UART_7O2 0x0E
#define UART_8O2 0x0F
#define UART_5E1 0x18
#define UART_6E1 0x19
#define UART_7E1 0x1A
#define UART_8E1 0x1B
#define UART_5E2 0x1C
#define UART_6E2 0x1D
#define UART_7E2 0x1E
#define UART_8E2 0x1F

/* Interrupts identification. */
#define INT_RLS    0x06  /* Receiver line status error */
#define INT_RTOUT  0x0c  /* Receiver timeout */
#define INT_RHR    0x04  /* Receiver Trigger */
#define INT_THR    0x02  /* Transmit Trigger */
#define INT_MODEM  0x00  /* Modem */
#define INT_PINS   0x30  /* Inoput pins change */
#define INT_XOFF   0x10  /* Receive XOff signal or special character */
#define INT_CTSRTS 0x20  /* CTS, RTS change of state from active (LOW) to inactive (HIGH) */

/* Configuration values for the I/Os. */
#define CONF_IO_DFLT    0x00
#define CONF_IO_LATCH   0x01

/* Interrupts configuration for the UART. */
#define CONF_INT_DFLT   0x00  /* Default, disbale all UART interrupts */
#define CONF_INT_CTSEN  0x80  /* Enable the CTS interrupt */
#define CONF_INT_RTSEN  0x40  /* Enable the RTS interrupt */
#define CONF_INT_XOFFEN 0x20  /* Enable the XOFF interrupt */
#define CONF_INT_MSEN   0x08  /* Enable the modem status interrupt */
#define CONF_INT_RLSEN  0x04  /* Enable the receiver line status interrupt */
#define CONF_INT_THREN  0x02  /* Enable the THR (Transmit Holding Register) interrupt */
#define CONF_INT_RHREN  0x01  /* Enable the RHR (Receive Holding Register) interrupt */

/* Registers definition. */
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
 * @class gnublin_module_sc16is7x0
 * @~english
 * @brief Class for accessing the SC16IS7x0 single uart via I2C.
 */
class gnublin_module_sc16is7x0 {

 protected :
    gnublin_i2c i2c;
    bool errorFlag;
    std::string errorMessage;

    void (*isrDataReceived)(char *, int);
    void (*isrSpaceAvailable)(int);

 public :
    gnublin_module_sc16is7x0(int address = 0x20, std::string filename = "/dev/i2c-1");
    virtual ~gnublin_module_sc16is7x0(void);
    int init(void);
    const char* getErrorMessage(void);
    bool fail(void);
    void setAddress(int address);
    void setDevicefile(std::string filename);
    int softReset(void);

    /* UART */
    int initUART(unsigned char value);
    int setBaudRate(unsigned int baud);
    int setDataFormat(unsigned char format);
    int writeByte(const char byte);
    int write(const char *buffer, unsigned int len);
    int readByte(char *byte);
    int read(char *buffer, unsigned int len);
    int rxAvailableData(void);
    int txAvailableSpace(void);
    int enableFifo(int value);
    int rxFifoSetTriggerLevel(unsigned int len);
    int txFifoSetTriggerLevel(unsigned int len);
    int rxEmptyFifo(void);
    int resetRxFifo(void);
    int resetTxFifo(void);

    /* Interrupts */
    int isIntPending(void);
    int whichInt(void);
    virtual int pollInt(void);
    int intIsrDataReceived(void (*isr)(char *, int));
    int intIsrSpaceAvailable(void (*isr)(int));
};

/* -------------------------------------------------------------------------- */

#endif

/* module_sc16is7x0.h ends here */
