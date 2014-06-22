// module_hd44780.cpp --- 
// 
// Filename     : module_hd44780.cpp
// Description  : Class for accessing a HD44780 compatible LCD.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Sun May  4 11:24:24 2014
// Version      : 1.0.0
// Last-Updated : Sun Jun 22 11:43:12 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 246
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
 */
gnublin_hd44780_driver::gnublin_hd44780_driver(int rs, int en, int d4, int d5, int d6, int d7) {

    _rs = rs;
    _en = en;
    _d4 = d4;
    _d5 = d5;
    _d6 = d6;
    _d7 = d7;
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
 */
gnublin_hd44780_driver_gpio::gnublin_hd44780_driver_gpio(int rs, int en, int d4, int d5, int d6, int d7)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    _gpio.pinMode(_rs, OUTPUT);
    _gpio.pinMode(_en, OUTPUT);
    _gpio.pinMode(_d4, OUTPUT);
    _gpio.pinMode(_d5, OUTPUT);
    _gpio.pinMode(_d6, OUTPUT);
    _gpio.pinMode(_d7, OUTPUT);
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
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_gpio::writeByte(unsigned char byte, int mode) {

    _gpio.digitalWrite(_rs, mode);

    /* High bits. */
    _gpio.digitalWrite(_d4, LOW);
    _gpio.digitalWrite(_d5, LOW);
    _gpio.digitalWrite(_d6, LOW);
    _gpio.digitalWrite(_d7, LOW);

    if ((byte & 0x10) == 0x10) {
        _gpio.digitalWrite(_d4, HIGH);
    }
    if ((byte & 0x20) == 0x20) {
        _gpio.digitalWrite(_d5, HIGH);
    }
    if ((byte & 0x40) == 0x40) {
        _gpio.digitalWrite(_d6, HIGH);
    }
    if ((byte & 0x80) == 0x80) {
        _gpio.digitalWrite(_d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    _gpio.digitalWrite(_en, HIGH);
    usleep(LCD_PULSE);
    _gpio.digitalWrite(_en, LOW);
    usleep(LCD_DELAY);

    /* Low bits. */
    _gpio.digitalWrite(_d4, LOW);
    _gpio.digitalWrite(_d5, LOW);
    _gpio.digitalWrite(_d6, LOW);
    _gpio.digitalWrite(_d7, LOW);

    if ((byte & 0x01) == 0x01) {
        _gpio.digitalWrite(_d4, HIGH);
    }
    if ((byte & 0x02) == 0x02) {
        _gpio.digitalWrite(_d5, HIGH);
    }
    if ((byte & 0x04) == 0x04) {
        _gpio.digitalWrite(_d6, HIGH);
    }
    if ((byte & 0x08) == 0x08) {
        _gpio.digitalWrite(_d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    _gpio.digitalWrite(_en, HIGH);
    usleep(LCD_PULSE);
    _gpio.digitalWrite(_en, LOW);
    usleep(LCD_DELAY);

    return 1;
}

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default pins used to drive the HD44780 LCD (4 bits data interface).
 * These are the pins of the MCP23017 I/O expander.
 * 0 : RS, 1 : EN : 1, 2 : D4, 3 : D5, 4 : D6, 5 : D7
 */
gnublin_hd44780_driver_mcp23017::gnublin_hd44780_driver_mcp23017(void)
    : gnublin_hd44780_driver(0, 1, 2, 3, 4, 5) {

    _mcp23017.pinMode(_rs, OUTPUT);
    _mcp23017.pinMode(_en, OUTPUT);
    _mcp23017.pinMode(_d4, OUTPUT);
    _mcp23017.pinMode(_d5, OUTPUT);
    _mcp23017.pinMode(_d6, OUTPUT);
    _mcp23017.pinMode(_d7, OUTPUT);
}


/**
 * @~english
 * @brief
 */
gnublin_hd44780_driver_mcp23017::~gnublin_hd44780_driver_mcp23017(void) {
}


/**
 * @~english
 * @brief Set the pins used to drive the HD44780 LCD (4 bits data interface).
 * These are the pins of the MCP23017 I/O expander.
 */
gnublin_hd44780_driver_mcp23017::gnublin_hd44780_driver_mcp23017(int rs, int en, int d4, int d5, int d6, int d7)
    : gnublin_hd44780_driver(rs, en, d4, d5, d6, d7) {

    _mcp23017.pinMode(_rs, OUTPUT);
    _mcp23017.pinMode(_en, OUTPUT);
    _mcp23017.pinMode(_d4, OUTPUT);
    _mcp23017.pinMode(_d5, OUTPUT);
    _mcp23017.pinMode(_d6, OUTPUT);
    _mcp23017.pinMode(_d7, OUTPUT);
}


/**
 * @~english
 * @brief Set the i2c address of the I/O expander.
 *
 * @param address The address to set.
 */
void gnublin_hd44780_driver_mcp23017::setAddress(int address) {

    _mcp23017.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file of the I/O expander.
 *
 * @param filename THe i2c device filename.
 */
void gnublin_hd44780_driver_mcp23017::setDevicefile(std::string filename) {

    _mcp23017.setDevicefile(filename);
}


/**
 * @~english
 * @brief Send byte to LCD data pins.
 *
 * @return 1 on success and -1 on error.
 */
int gnublin_hd44780_driver_mcp23017::writeByte(unsigned char byte, int mode) {

    _mcp23017.digitalWrite(_rs, mode);

    /* High bits. */
    _mcp23017.digitalWrite(_d4, LOW);
    _mcp23017.digitalWrite(_d5, LOW);
    _mcp23017.digitalWrite(_d6, LOW);
    _mcp23017.digitalWrite(_d7, LOW);

    if ((byte & 0x10) == 0x10) {
        _mcp23017.digitalWrite(_d4, HIGH);
    }
    if ((byte & 0x20) == 0x20) {
        _mcp23017.digitalWrite(_d5, HIGH);
    }
    if ((byte & 0x40) == 0x40) {
        _mcp23017.digitalWrite(_d6, HIGH);
    }
    if ((byte & 0x80) == 0x80) {
        _mcp23017.digitalWrite(_d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    _mcp23017.digitalWrite(_en, HIGH);
    usleep(LCD_PULSE);
    _mcp23017.digitalWrite(_en, LOW);
    usleep(LCD_DELAY);

    /* Low bits. */
    _mcp23017.digitalWrite(_d4, LOW);
    _mcp23017.digitalWrite(_d5, LOW);
    _mcp23017.digitalWrite(_d6, LOW);
    _mcp23017.digitalWrite(_d7, LOW);

    if ((byte & 0x01) == 0x01) {
        _mcp23017.digitalWrite(_d4, HIGH);
    }
    if ((byte & 0x02) == 0x02) {
        _mcp23017.digitalWrite(_d5, HIGH);
    }
    if ((byte & 0x04) == 0x04) {
        _mcp23017.digitalWrite(_d6, HIGH);
    }
    if ((byte & 0x08) == 0x08) {
        _mcp23017.digitalWrite(_d7, HIGH);
    }

    /* Toggle EN pin. */
    usleep(LCD_DELAY);
    _mcp23017.digitalWrite(_en, HIGH);
    usleep(LCD_PULSE);
    _mcp23017.digitalWrite(_en, LOW);
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

    _driver = driver;
    _errorFlag = false;
    _rows = 2;
    _cols = 16;
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

    _driver = driver;
    _errorFlag = false;
    _rows = rows;
    _cols = cols;
}


/**
 * @~english
 * @brief Set the cursor to the given row.
 *
 * @param row The row on which to set the cursor.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::_setRow(int row) {

    _errorFlag = false;

    if (row < 1 || row > _rows) {
        _errorFlag = true;
        sprintf(const_cast<char*>(_errorMessage.c_str()), "Row number is not between 1 and %d\n", _rows);
        return -1;
    }

    if (_driver->writeByte(LCD_ROWS[row - 1], LCD_CMD) < 0) {
        _errorFlag = true;
        _errorMessage = "driver.writeByte Error\n";
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
int gnublin_module_hd44780::_setCol(int col) {

    _errorFlag = false;
    _crtCol = 0;

    if (col < 1 || col > _cols) {
        _errorFlag = true;
        sprintf(const_cast<char*>(_errorMessage.c_str()), "Col number is not between 1 and %d\n", _cols);
        return -1;
    }

    for (int i = 0; i < col; i++) {
        if (_driver->writeByte(' ', LCD_DATA) < 0) {
            _errorFlag = true;
            _errorMessage = "driver.writeByte Error\n";
            return -1;
        }
    }
    _crtCol = col;

    return 1;
}


/**
 * @~english
 * @brief Print a string at the current row and column.
 *
 * @param buffer The string to print.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::_print(char *buffer) {

    _errorFlag = false;
    int length = strlen(buffer);
    if (length > _cols - _crtCol) {
        length = _cols - _crtCol;
    }

    for (int i = 0; i < length; i++) {
        if (_driver->writeByte(buffer[i], LCD_DATA) < 0) {
            _errorFlag = true;
            _errorMessage = "driver.writeByte Error\n";
            return -1;
        }
    }

    int padLength = _cols - _crtCol - length;
    for (int i = length; i < padLength; i++) {
        if (_driver->writeByte(' ', LCD_DATA) < 0) {
            _errorFlag = true;
            _errorMessage = "driver.writeByte Error\n";
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
        if (int result = _driver->writeByte(initBytes[i], LCD_CMD) < 0) {
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

    return _errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_hd44780::fail(void) {

    return _errorFlag;
}


/**
 * @~english
 * @brief Set an offset to the display
 *
 * @param col Number of column for the offset.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::offset(int col) {

    return _setCol(col);
}


/**
 * @~english
 * @brief Print a string on the display at the current cursor position.
 *
 * @param buffer The string to print.
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::print(char *buffer) {

    _errorFlag = false;

    if (_setRow(1) < 0) {
        _errorFlag = true;
        _errorMessage = "set row Error\n";
        return -1;
    }
    _crtCol = 0;

    return _print(buffer);
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

    _errorFlag = false;

    if (_setRow(row) < 0) {
        _errorFlag = true;
        _errorMessage = "set row Error\n";
        return -1;
    }
    _crtCol = 0;

    return _print(buffer);
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

    _errorFlag = false;

    if (_setRow(row) < 0) {
        _errorFlag = true;
        _errorMessage = "set row Error\n";
        return -1;
    }

    if (_setCol(col) < 0) {
        _errorFlag = true;
        _errorMessage = "set col Error\n";
        return -1;
    }

    return _print(buffer);
}


/**
 * @~english
 * @brief Clear the display.
 *
 * @return 1 on success and -1 on error.
 */
int gnublin_module_hd44780::clear(void) {

    _errorFlag = false;
    
    if (_driver->writeByte(0x01, LCD_CMD) < 0) {
        _errorFlag = true;
        _errorMessage = "driver.writeByte Error\n";
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

    _errorFlag = false;
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

    if (_driver->writeByte(ctrlByte, LCD_CMD) < 0) {
        _errorFlag = true;
        _errorMessage = "driver.writeByte Error\n";
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
    
    _errorFlag = false;
    
    if (_driver->writeByte(LCD_HOME, LCD_CMD) < 0) {
        _errorFlag = true;
        _errorMessage = "driver.writeByte Error\n";
        return -1;
    }

    _crtCol = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */

// 
// module_hd44780.cpp ends here
