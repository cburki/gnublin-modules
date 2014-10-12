/* module_hd44780.h --- 
 * 
 * Filename     : module_hd44780.h
 * Description  : Class for accessing a HD44780 compatible LCD.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sat May  3 18:21:57 2014
 * Version      : 1.2.0
 * Last-Updated : Sun Oct 12 15:05:05 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 111
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
 * 2014-06-22 : Added driver for the sc16is750 device.
 * 2014-09-14 : Added driver for the 74hc595 shift register.
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

#ifndef GNUBLIN_MODULE_HD44780
#define GNUBLIN_MODULE_HD44780

/* -------------------------------------------------------------------------- */

#include "gnublin.h"
#include "module_mcp23017.h"
#include "module_sc16is750.h"

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_hd44780_driver
 * @~english
 * @brief Abstract class defining a HD44780 driver.
 */
class gnublin_hd44780_driver {

 protected :
    int rs;
    int en;
    int d4;
    int d5;
    int d6;
    int d7;

 public :
    gnublin_hd44780_driver(int rs, int en, int d4, int d5, int d6, int d7);
    virtual ~gnublin_hd44780_driver(void);
    virtual int writeByte(unsigned char byte, int mode) = 0;

};

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_hd44780_driver_gpio
 * @~english
 * @brief Class for accessing a HD44780 compatible LCD connected directly to GPIO.
 */
class gnublin_hd44780_driver_gpio : public gnublin_hd44780_driver {

 private :
    gnublin_gpio gpio;

 public :
    gnublin_hd44780_driver_gpio(int rs, int en, int d4, int d5, int d6, int d7);
    ~gnublin_hd44780_driver_gpio(void);
    int writeByte(unsigned char byte, int mode);
};

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_hd44780_driver_74hc595
 * @~english
 * @brief Class for accessing a HD44780 compatible LCD connected via a 74HC595
 * shift register.
 */
class gnublin_hd44780_driver_74hc595 : public gnublin_hd44780_driver {

 private :
    gnublin_gpio gpio;
    int ds;
    int shcp;
    int stcp;

    void shiftByte(unsigned char value, int msbFirst = 1);
    void latchByte(void);

 public :
    gnublin_hd44780_driver_74hc595(int rs, int en, int d4, int d5, int d6, int d7, int ds, int shcp, int stcp);
    ~gnublin_hd44780_driver_74hc595(void);
    int writeByte(unsigned char byte, int mode);
};

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_hd44780_driver_mcp23017
 * @~english
 * @brief Class for accessing a HD44780 compatible LCD connected via a
 * MCP23017 port expander.
 */
class gnublin_hd44780_driver_mcp23017 : public gnublin_hd44780_driver {

 private :
    gnublin_module_mcp23017 mcp23017;

 public :
    gnublin_hd44780_driver_mcp23017(int rs, int en, int d4, int d5, int d6, int d7, int i2cAddress = 0x20, std::string i2cFilename = "/dev/i2c-1");
    ~gnublin_hd44780_driver_mcp23017(void);
    void setAddress(int address);
    void setDevicefile(std::string filename);
    int writeByte(unsigned char byte, int mode);
};

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_hd44780_driver_sc16is750
 * @~english
 * @brief Class for accessing a HD44780 compatible LCD connected via the
 * IO of a SC16IS750 device.
 */
class gnublin_hd44780_driver_sc16is750 : public gnublin_hd44780_driver {

 private :
    gnublin_module_sc16is750 sc16is750;

 public :
    gnublin_hd44780_driver_sc16is750(int rs, int en, int d4, int d5, int d6, int d7, int i2cAddress = 0x20, std::string i2cFilename = "/dev/i2c-1");
    ~gnublin_hd44780_driver_sc16is750(void);
    void setAddress(int address);
    void setDevicefile(std::string filename);
    int writeByte(unsigned char byte, int mode);
};

/* -------------------------------------------------------------------------- */

/**
 * @class gnnublin_module_hd44780
 * @~english
 * @brief Class for accessing a HD44780 compatilble LCD.
 */
class gnublin_module_hd44780 {
    
 private :
    int rows;
    int cols;
    int crtCol;
    gnublin_hd44780_driver *driver;
    bool errorFlag;
    std::string errorMessage;

    int setRow(int row);
    int setCol(int col);
    int write(char *buffer);

 public :
    gnublin_module_hd44780(gnublin_hd44780_driver *driver);
    gnublin_module_hd44780(gnublin_hd44780_driver *driver, int rows, int cols);
    int init(void);
    const char* getErrorMessage(void);
    bool fail(void);

    int offset(int num);
    int print(char *buffer);
    int print(char *buffer, int row);
    int print(char *buffer, int row, int offset);
    int clear(void);
    int returnHome(void);
    int controlDisplay(int power, int cursor, int blink);
};

/* -------------------------------------------------------------------------- */

#endif

/* module_hd44780.h ends here */
