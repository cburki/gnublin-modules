/* module_sc16is750.h --- 
 * 
 * Filename     : module_sc16is750.h
 * Description  : Class for accessing gnublin SC16IS750 single UART.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Thu May 29 15:08:43 2014
 * Version      : 
 * Last-Updated : Sun Oct 12 15:40:17 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 126
 * URL          : 
 * Keywords     : 
 * Compatibility: 
 * 
 */

/* Commentary   : 
 *
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

#ifndef GNUBLIN_MODULE_SC16IS750
#define GNUBLIN_MODULE_SC16IS750

/* -------------------------------------------------------------------------- */

#include "gnublin.h"
#include "module_sc16is7x0.h"

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_module_sc16is750
 * @~english
 * @brief Class for accessing the SC16IS750 single uart via I2C.
 */
class gnublin_module_sc16is750 : public gnublin_module_sc16is7x0 {

 private :
    unsigned char ioLatchReg;
    void (*isrIO)(int, int);

 public :
    gnublin_module_sc16is750(int address = 0x20, std::string filename = "/dev/i2c-1");
    int init(void);

    /* GPIOs */
    int initIO(unsigned char value);
    int pinMode(int pin, std::string direction);
    int portMode(std::string direction);
    int digitalWrite(int pin, int value);
    int digitalRead(int pin);
    int readState(int pin);
    int writePort(unsigned char value);
    unsigned char readPort(void);
    int pinIntEnable(int pin, int value);
    int portIntEnable(int value);
    unsigned char readIntFlagPort(void);

    /* Interrupts */
    int pollInt(void);
    int intIsrIO(void (*isr)(int, int));
};

/* -------------------------------------------------------------------------- */

#endif

/* module_sc16is750.h ends here */
