// module_hd44780.cpp --- 
// 
// Filename     : module_hd44780.cpp
// Description  : Class for accessing a HD44780 compatible LCD.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Sun May  4 11:24:24 2014
// Version      : 1.1.0
// Last-Updated : Sun Oct 12 15:48:45 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 407
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
// 2014-06-22 : Added driver for the sc16is750 device. 
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

#include <stdio.h>
#include <unistd.h>

#include "module_hd44780.h"

/* -------------------------------------------------------------------------- */

#define LCD_CMD   0      /* RS pin level for command mode. */
#define LCD_DATA  1      /* RS pin level for data mode. */
#define LCD_ROW_1 0x80   /* LCD register address for line 1. */
#define LCD_ROW_2 0xc0   /* LCD register address for line 2. */
#define LCD_ROW_3 0x94   /* LCD register address for line 3. */
#define LCD_ROW_4 0xd4   /* LCD register address for line 4. */
const unsigned char LCD_ROWS[] = { LCD_ROW_1, LCD_ROW_2, LCD_ROW_3, LCD_ROW_4 };

#define LCD_CLEAR  0x01
#define LCD_HOME   0x02
#define LCD_CTRL   0x08
#define LCD_ONOFF  0x0c
#define LCD_CURSOR 0x0a
#define LCD_BLINK  0x09

#define LCD_PULSE 50     /* micro seconds. */
#define LCD_DELAY 50

/* -------------------------------------------------------------------------- */

/**
  * @~english
  * @brief Set the pins used to drive the HD44780 LCD (4 bits data interface).
  *
  * @param rs The RS pin.
  * @param en The EN pin.
  * @param d4 The D4 pin.
  * @param d5 The D5 pin.
  * @param d6 The D6 pin.
  * @param d7 The D7 pin.
  */
gnublin_hd44780_driver::gnublin_hd44780_driver(int rs, int en, int d4, int d5, int d6, int d7) {
 
     this->rs = rs;
     this->en = en;
     this->d4 = d4;
     this->d5 = d5;
     this->d6 = d6;
     this->d7 = d7;
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver::~gnublin_hd44780_driver(void) {
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the pins used to drive the HD44780 LCD (4 bits data interface).
  *
  * @param rs The RS pin.
  * @param en The EN pin.
  * @param d4 The D4 pin.
  * @param d5 The D5 pin.
  * @param d6 The D6 pin.
  * @param d7 The D7 pin.
 */
gnublin_hd44780_driver_gpio::gnublin_hd44780_driver_gpio(int rs, int en, int d4, int d5, int d6, int d7)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    gpio.pinMode(rs, OUTPUT);
    gpio.pinMode(en, OUTPUT);
    gpio.pinMode(d4, OUTPUT);
    gpio.pinMode(d5, OUTPUT);
    gpio.pinMode(d6, OUTPUT);
    gpio.pinMode(d7, OUTPUT);
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver_gpio::~gnublin_hd44780_driver_gpio(void) {
}


/**
 * @~english
 * @brief Send byte to LCD data pins.
 *
 * @param byte The byte to write to the LCD.
 * @param mode The mode for the byte to write. Either data (LCD_DATA) or command (LCD_CMD).
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_gpio::writeByte(unsigned char byte, int mode) {

    gpio.digitalWrite(rs, mode);

    /* High bits. */
    gpio.digitalWrite(d4, LOW);
    gpio.digitalWrite(d5, LOW);
    gpio.digitalWrite(d6, LOW);
    gpio.digitalWrite(d7, LOW);

    if ((byte & 0x10) == 0x10) {
        gpio.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x20) == 0x20) {
        gpio.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x40) == 0x40) {
        gpio.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x80) == 0x80) {
        gpio.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    gpio.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    gpio.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    /* Low bits. */
    gpio.digitalWrite(d4, LOW);
    gpio.digitalWrite(d5, LOW);
    gpio.digitalWrite(d6, LOW);
    gpio.digitalWrite(d7, LOW);

    if ((byte & 0x01) == 0x01) {
        gpio.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x02) == 0x02) {
        gpio.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x04) == 0x04) {
        gpio.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x08) == 0x08) {
        gpio.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    gpio.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    gpio.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the pins used to drive the 74HC595 shift register and set the output
 * bit (QX) of the shift register for driving the HD44780 LCD (4 bits data interface).
  *
  * @param rs The RS bit number for the shift register output.
  * @param en The EN bit number for the shift register output.
  * @param d4 The D4 bit number for the shift register output.
  * @param d5 The D5 bit number for the shift register output.
  * @param d6 The D6 bit number for the shift register output.
  * @param d7 The D7 bit number for the shift register output.
  * @param ds The DS pin for driving the 74HC595.
  * @param shcp The SHCP pin for driving the 74HC595.
  * @param stcp THe STCP pin for driving the 74HC595.
 */
gnublin_hd44780_driver_74hc595::gnublin_hd44780_driver_74hc595(int rs, int en, int d4, int d5, int d6, int d7, int ds, int shcp, int stcp)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    this->ds = ds;
    this->shcp = shcp;
    this->stcp = stcp;

    gpio.pinMode(ds, OUTPUT);
    gpio.pinMode(shcp, OUTPUT);
    gpio.pinMode(stcp, OUTPUT);

    gpio.digitalWrite(ds, LOW);
    gpio.digitalWrite(shcp, LOW);
    gpio.digitalWrite(stcp, LOW);
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver_74hc595::~gnublin_hd44780_driver_74hc595(void) {
}


/**
 * @~english
 * @brief
 */
void gnublin_hd44780_driver_74hc595::shiftByte(unsigned char value, int msbFirst) {

    for (int i = 0; i < 8; i++) {

        if (msbFirst == 1) {  /* MSB First */
            if (value & 0x80) {
                gpio.digitalWrite(ds, HIGH);
            }
            else {
                gpio.digitalWrite(ds, LOW);
            }
        }
        else {                /* LSB First */
            if (value & 1) {
                gpio.digitalWrite(ds, HIGH);
            }
            else {
                gpio.digitalWrite(ds, LOW);
            }
        }

        gpio.digitalWrite(shcp, HIGH);
        gpio.digitalWrite(ds, LOW);
        gpio.digitalWrite(shcp, LOW);

        if (msbFirst == 1) {  /* MSB First */
            value <<= 1;
        }
        else {                /* LSB First */
            value >>= 1;
        }
    }
}


/**
 * @~english
 * @brief
 */
void gnublin_hd44780_driver_74hc595::latchByte(void) {

    gpio.digitalWrite(stcp, HIGH);
    gpio.digitalWrite(stcp, LOW);
}


/**
 * @~english
 * @brief Send byte to LCD data pins.
 *
 * @param byte The byte to write to the LCD.
 * @param mode The mode for the byte to write. Either data (LCD_DATA) or command (LCD_CMD).
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_74hc595::writeByte(unsigned char byte, int mode) {

    unsigned char value = 0x00 | (mode << rs);

    /* High bits. */
    if ((byte & 0x10) == 0x10) {
        value |= (1 << d4);
    }
    if ((byte & 0x20) == 0x20) {
        value |= (1 << d5);
    }
    if ((byte & 0x40) == 0x40) {
        value |= (1 << d6);
    }
    if ((byte & 0x80) == 0x80) {
        value |= (1 << d7);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    value |= (1 << en);
    shiftByte(value);
    latchByte();
    usleep(LCD_PULSE);
    value = 0x00 | (mode << rs);
    shiftByte(value);
    latchByte();
    usleep(LCD_DELAY);

    /* Low bits. */
    value = 0x00 | (mode << rs);
    if ((byte & 0x01) == 0x01) {
        value |= (1 << d4);
    }
    if ((byte & 0x02) == 0x02) {
        value |= (1 << d5);
    }
    if ((byte & 0x04) == 0x04) {
        value |= (1 << d6);
    }
    if ((byte & 0x08) == 0x08) {
        value |= (1 << d7);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    value |= (1 << en);
    shiftByte(value);
    latchByte();
    usleep(LCD_PULSE);
    value = 0x00 | (mode << rs);
    shiftByte(value);
    latchByte();
    usleep(LCD_DELAY);
    
    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the pins used to drive the HD44780 LCD (4 bits data interface).
 * These are the pins of the MCP23017 I/O expander.
  *
  * @param rs The RS pin.
  * @param en The EN pin.
  * @param d4 The D4 pin.
  * @param d5 The D5 pin.
  * @param d6 The D6 pin.
  * @param d7 The D7 pin.
  * @param i2cAddress The i2c address of the device.
  * @param i2cFilename The i2c device file
 */
gnublin_hd44780_driver_mcp23017::gnublin_hd44780_driver_mcp23017(int rs, int en, int d4, int d5, int d6, int d7, int i2cAddress, std::string i2cFilename)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    mcp23017 = gnublin_module_mcp23017(i2cAddress, i2cFilename);

    mcp23017.pinMode(rs, OUTPUT);
    mcp23017.pinMode(en, OUTPUT);
    mcp23017.pinMode(d4, OUTPUT);
    mcp23017.pinMode(d5, OUTPUT);
    mcp23017.pinMode(d6, OUTPUT);
    mcp23017.pinMode(d7, OUTPUT);
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver_mcp23017::~gnublin_hd44780_driver_mcp23017(void) {
}


/**
 * @~english
 * @brief Set the i2c address of the I/O expander.
 *
 * @param address The address to set.
 */
void gnublin_hd44780_driver_mcp23017::setAddress(int address) {

    mcp23017.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file of the I/O expander.
 *
 * @param filename THe i2c device filename.
 */
void gnublin_hd44780_driver_mcp23017::setDevicefile(std::string filename) {

    mcp23017.setDevicefile(filename);
}


/**
 * @~english
 * @brief Send byte to LCD data pins.
 *
 * @param byte The byte to write to the LCD.
 * @param mode The mode for the byte to write. Either data (LCD_DATA) or command (LCD_CMD).
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_mcp23017::writeByte(unsigned char byte, int mode) {

    mcp23017.digitalWrite(rs, mode);

    /* High bits. */
    mcp23017.digitalWrite(d4, LOW);
    mcp23017.digitalWrite(d5, LOW);
    mcp23017.digitalWrite(d6, LOW);
    mcp23017.digitalWrite(d7, LOW);

    if ((byte & 0x10) == 0x10) {
        mcp23017.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x20) == 0x20) {
        mcp23017.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x40) == 0x40) {
        mcp23017.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x80) == 0x80) {
        mcp23017.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    mcp23017.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    mcp23017.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    /* Low bits. */
    mcp23017.digitalWrite(d4, LOW);
    mcp23017.digitalWrite(d5, LOW);
    mcp23017.digitalWrite(d6, LOW);
    mcp23017.digitalWrite(d7, LOW);

    if ((byte & 0x01) == 0x01) {
        mcp23017.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x02) == 0x02) {
        mcp23017.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x04) == 0x04) {
        mcp23017.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x08) == 0x08) {
        mcp23017.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    mcp23017.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    mcp23017.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the pins used to drive the HD44780 LCD (4 bits data interface).
 * These are the pins of the SC16IS750 device.
  *
  * @param rs The RS pin.
  * @param en The EN pin.
  * @param d4 The D4 pin.
  * @param d5 The D5 pin.
  * @param d6 The D6 pin.
  * @param d7 The D7 pin.
  * @param i2cAddress The i2c address of the device.
  * @param i2cFilename The i2c device file
 */
gnublin_hd44780_driver_sc16is750::gnublin_hd44780_driver_sc16is750(int rs, int en, int d4, int d5, int d6, int d7, int i2cAddress, std::string i2cFilename)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    sc16is750 = gnublin_module_sc16is750(i2cAddress, i2cFilename);

    sc16is750.pinMode(rs, OUTPUT);
    sc16is750.pinMode(en, OUTPUT);
    sc16is750.pinMode(d4, OUTPUT);
    sc16is750.pinMode(d5, OUTPUT);
    sc16is750.pinMode(d6, OUTPUT);
    sc16is750.pinMode(d7, OUTPUT);
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver_sc16is750::~gnublin_hd44780_driver_sc16is750(void) {
}


/**
 * @~english
 * @brief Set the i2c address of the I/O expander.
 *
 * @param address The address to set.
 */
void gnublin_hd44780_driver_sc16is750::setAddress(int address) {

    sc16is750.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file of the I/O expander.
 *
 * @param filename THe i2c device filename.
 */
void gnublin_hd44780_driver_sc16is750::setDevicefile(std::string filename) {

    sc16is750.setDevicefile(filename);
}


/**
 * @~english
 * @brief Send byte to LCD data pins.
 *
 * @param byte The byte to write to the LCD.
 * @param mode The mode for the byte to write. Either data (LCD_DATA) or command (LCD_CMD).
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_sc16is750::writeByte(unsigned char byte, int mode) {

    sc16is750.digitalWrite(rs, mode);

    /* High bits. */
    sc16is750.digitalWrite(d4, LOW);
    sc16is750.digitalWrite(d5, LOW);
    sc16is750.digitalWrite(d6, LOW);
    sc16is750.digitalWrite(d7, LOW);

    if ((byte & 0x10) == 0x10) {
        sc16is750.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x20) == 0x20) {
        sc16is750.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x40) == 0x40) {
        sc16is750.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x80) == 0x80) {
        sc16is750.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    sc16is750.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    sc16is750.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    /* Low bits. */
    sc16is750.digitalWrite(d4, LOW);
    sc16is750.digitalWrite(d5, LOW);
    sc16is750.digitalWrite(d6, LOW);
    sc16is750.digitalWrite(d7, LOW);

    if ((byte & 0x01) == 0x01) {
        sc16is750.digitalWrite(d4, HIGH);
    }
    if ((byte & 0x02) == 0x02) {
        sc16is750.digitalWrite(d5, HIGH);
    }
    if ((byte & 0x04) == 0x04) {
        sc16is750.digitalWrite(d6, HIGH);
    }
    if ((byte & 0x08) == 0x08) {
        sc16is750.digitalWrite(d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    sc16is750.digitalWrite(en, HIGH);
    usleep(LCD_PULSE);
    sc16is750.digitalWrite(en, LOW);
    usleep(LCD_DELAY);

    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default number of rows
 * and columns. rows : 2, cols = 16
 *
 * @param driver The driver object used to drive the LCD.
 */
gnublin_module_hd44780::gnublin_module_hd44780(gnublin_hd44780_driver *driver) {

    this->driver = driver;
    errorFlag = false;
    rows = 2;
    cols = 16;
}


/**
 * @~english
 * @brief Set the number of rows and columns.
 *
 * @param driver The driver object used to drive the LCD.
 * @param rows The number of rows of the display.
 * @param cols The number of columns of the display.
 */
gnublin_module_hd44780::gnublin_module_hd44780(gnublin_hd44780_driver *driver, int rows, int cols) {

    this->driver = driver;
    errorFlag = false;
    this->rows = rows;
    this->cols = cols;
}


/**
 * @~english
 * @brief Set the cursor to the given row.
 *
 * @param row The row on which to set the cursor.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::setRow(int row) {

    errorFlag = false;

    if (row < 1 || row > rows) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Row number is not between 1 and %d\n", rows);
        return -1;
    }

    if (driver->writeByte(LCD_ROWS[row - 1], LCD_CMD) < 0) {
        errorFlag = true;
        errorMessage = "driver.writeByte Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set the cursor to the given column.
 *
 * @param col The column on which to set the cursor.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::setCol(int col) {

    errorFlag = false;
    crtCol = 0;

    if (col < 1 || col > cols) {
        errorFlag = true;
        sprintf(const_cast<char*>(errorMessage.c_str()), "Col number is not between 1 and %d\n", cols);
        return -1;
    }

    for (int i = 1; i < col; i++) {
        if (driver->writeByte(' ', LCD_DATA) < 0) {
            errorFlag = true;
            errorMessage = "driver.writeByte Error\n";
            return -1;
        }
    }
    crtCol = col;

    return 1;
}


/**
 * @~english
 * @brief Print a string at the current row and column.
 *
 * @param buffer The string to print.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::write(char *buffer) {

    errorFlag = false;
    int length = strlen(buffer);
    if (length > cols - crtCol + 1) {
        length = cols - crtCol + 1;
    }

    for (int i = 0; i < length; i++) {
        if (driver->writeByte(buffer[i], LCD_DATA) < 0) {
            errorFlag = true;
            errorMessage = "driver.writeByte Error\n";
            return -1;
        }
    }

    int padLength = cols - crtCol - length;
    for (int i = length; i < padLength; i++) {
        if (driver->writeByte(' ', LCD_DATA) < 0) {
            errorFlag = true;
            errorMessage = "driver.writeByte Error\n";
            return -1;
        }
    }

    return 1;
}


/**
 * @~english
 * @brief Initialize the display.
 */
int gnublin_module_hd44780::init(void) {

    unsigned char initBytes[] = { 0x33, 0x32, 0x28, 0x0c, 0x06, 0x01 };
    /* 0x33, 0x32 : Initialization sequence.
       0x28       : Set interface (4 bits, 2 lines)
       0x0c       : Display on
       0x06       : Move cursor right
       0x01       : Clear display
    */

    for (unsigned int i = 0; i < sizeof(initBytes); i++) {
        if (int result = driver->writeByte(initBytes[i], LCD_CMD) < 0) {
            return result;
        }
    }

    return 1;
}


/**
 * @~english
 * @brief Get the last error message.
 *
 * @return The error message as c-string.
 */
const char* gnublin_module_hd44780::getErrorMessage(void) {

    return errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_hd44780::fail(void) {

    return errorFlag;
}


/**
 * @~english
 * @brief Set an offset to the display
 *
 * @param col Number of column for the offset.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::offset(int col) {

    return setCol(col);
}


/**
 * @~english
 * @brief Print a string on the display at the current cursor position.
 *
 * @param buffer The string to print.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::print(char *buffer) {

    errorFlag = false;

    if (setRow(1) < 0) {
        errorFlag = true;
        errorMessage = "set row Error\n";
        return -1;
    }
    crtCol = 0;

    return write(buffer);
}


/**
 * @~english
 * @brief Print a string on the display at the given row.
 *
 * @param buffer The string to print.
 * @param row The row at which to print the string.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::print(char *buffer, int row) {

    errorFlag = false;

    if (setRow(row) < 0) {
        errorFlag = true;
        errorMessage = "set row Error\n";
        return -1;
    }
    crtCol = 0;

    return write(buffer);
}


/**
 * @~english
 * @brief Pritn a string on the display athe given row and column.
 *
 * @param buffer The string to print.
 * @param row The row at which to print the string.
 * @param col The column at which to print the string.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::print(char *buffer, int row, int col) {

    errorFlag = false;

    if (setRow(row) < 0) {
        errorFlag = true;
        errorMessage = "set row Error\n";
        return -1;
    }

    if (setCol(col) < 0) {
        errorFlag = true;
        errorMessage = "set col Error\n";
        return -1;
    }

    return write(buffer);
}


/**
 * @~english
 * @brief Clear the display.
 *
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::clear(void) {

    errorFlag = false;
    
    if (driver->writeByte(0x01, LCD_CMD) < 0) {
        errorFlag = true;
        errorMessage = "driver.writeByte Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Set display parameters.
 *
 * @param power Switch display on or off.
 * @param cursor Switch cursor on or off.
 * @param blink Swith the blinking of the cursor on or off.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::controlDisplay(int power, int cursor, int blink) {

    errorFlag = false;
    unsigned char ctrlByte = LCD_CTRL;

    if (power) {
        ctrlByte |= LCD_ONOFF;
    }
    if (cursor) {
        ctrlByte |= LCD_CURSOR;
    }
    if (blink) {
        ctrlByte |= LCD_BLINK;
    }

    if (driver->writeByte(ctrlByte, LCD_CMD) < 0) {
        errorFlag = true;
        errorMessage = "driver.writeByte Error\n";
        return -1;
    }

    return 1;
}


/**
 * @~english
 * @brief Return the cursor to the home position (0, 0).
 *
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::returnHome(void) {
    
    errorFlag = false;
    
    if (driver->writeByte(LCD_HOME, LCD_CMD) < 0) {
        errorFlag = true;
        errorMessage = "driver.writeByte Error\n";
        return -1;
    }

    crtCol = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_hd44780.cpp ends here
