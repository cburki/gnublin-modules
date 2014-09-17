/* module_mcp230xx.h --- 
 * 
 * Filename     : module_mcp230xx.h
 * Description  : Class for accessing gnublin MCP23017 and MCP23009 port expander.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sun Apr 27 15:27:32 2014
 * Version      : 1.0.0
 * Last-Updated : Mon Sep 15 21:17:59 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 17
 * URL          : 
 * Keywords     : 
 * Compatibility: 
 * 
 */

/* Commentary   : 
 * 
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

#ifndef GNUBLIN_MODULE_MCP230XX
#define GNUBLIN_MODULE_MCP230XX

/* -------------------------------------------------------------------------- */

#include "gnublin.h"

/* -------------------------------------------------------------------------- */

#define CONF_INTLOW    0x00  /* Interrupt output pins are active low. */
#define CONF_INTHIGH   0x02  /* Interrupt output pins are active high. */
#define CONF_INTMIRROR 0x40  /* Mirror the interrupt for port A and B. */

#define INT_CHANGE "change"
#define INT_HIGH   "high"
#define INT_LOW    "low"
#define INT_NONE   "none"

#define MAX_PINS  16
#define MAX_PORTS 2

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_module_mcp230xx
 * @~english
 * @brief Class for accessing the MCP23017 and MCP23009 port expander via I2C.
 */
class gnublin_module_mcp230xx {

  protected :
    gnublin_i2c _i2c;
    bool _errorFlag;
    std::string _errorMessage;

    int _pins;
    int _ports;
    int _registerShift;  /* Left shift when accessing registers. */

    void (*_isr)(int, int, int);
    void (*_pinIsr[MAX_PINS])(int);
    void (*_portIsr[MAX_PORTS])(int, int);

 public :
    gnublin_module_mcp230xx(int ports, int pins, int address = 0x20, std::string filename = "/dev/i2c-1");
    int init(unsigned char value);
    const char* getErrorMessage(void);
    bool fail(void);
    void setAddress(int address);
    void setDevicefile(std::string filename);
    int pinMode(int pin, std::string direction);
    int portMode(int port, std::string direction);
    int digitalWrite(int pin, int value);
    int digitalRead(int pin);
    int readState(int pin);
    int writePort(int port, unsigned char value);
    unsigned char readPort(int port);

    int pinIntMode(int pin, std::string mode);
    int portIntMode(int port, std::string mode);
    int pinPullUpMode(int pin, int value);
    int portPullUpMode(int port, int value);
    int pinPolarityMode(int pin, int value);
    int portPolarityMode(int port, int value);
    int digitalIntRead(int pin);
    unsigned char readIntPort(int port);
    unsigned char readIntFlagPort(int port);

    int pollInt(void);
    int intIsr(void (*isr)(int, int, int));
    int pinIntIsr(int pin, void (*isr)(int));
    int portIntIsr(int port, void (*isr)(int, int));
};

/* -------------------------------------------------------------------------- */

#endif

/* module_mcp230xx.h ends here */
