// module_mcp23009.cpp --- 
// 
// Filename     : module_mcp23009.cpp
// Description  : Class for accessing gnublin MCP23009 port expander.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Sun Apr 27 15:39:20 2014
// Version      : 1.0.0
// Last-Updated : Thu Sep 11 11:10:10 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 12
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

#include "module_mcp23009.h"

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 *
 * @param address The i2c address.
 * @param filename The i2c device file.
 */
gnublin_module_mcp23009::gnublin_module_mcp23009(int address, std::string filename)
    : gnublin_module_mcp230xx(0, 8, address, filename) {

}


/**
 * @~english
 * @brief Set the mode of the port. All pins of the port are set with
 * the same mode.
 *
 * @param direction The direction (in or out) to set to the port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp23009::portMode(std::string direction) {

    return gnublin_module_mcp230xx::portMode(0, direction);
}


/**
 * @~english
 * @brief Write a value (byte) to the port.
 *
 * @param value The byte to write to the port.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp23009::writePort(unsigned char value) {

    return gnublin_module_mcp230xx::writePort(0, value);
}


/**
 * @~english
 * @brief Read a value (byte) from the port.
 *
 * @return The value read from the port.
 */
unsigned char gnublin_module_mcp23009::readPort(void) {

    return gnublin_module_mcp230xx::readPort(0);
}


/**
 * @~english
 * @brief Set the interrupt mode of the port.
 *
 * @param value The mode for the interrupt which could be change, high, low or none.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp23009::portIntMode(std::string mode) {

    return gnublin_module_mcp230xx::portIntMode(0, mode);
}


/**
 * @~english
 * @brief Set the pull-up resistor mode (enable or disable) of the port.
 * All pins of the port are set with the same value.
 *
 * @param value The value to set or clear the pull-up mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp23009::portPullUpMode(int value) {

    return gnublin_module_mcp230xx::portPullUpMode(0, value);
}


/**
 * @~english
 * @brief Set the input polarity mode (enable or disable) of the port. If the pin
 * is set, the GPIO will reflect the inverted value of the input pin.
 * All pins of the port are set with the same value.
 *
 * @param value The value to set or clear the polarity mode.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_mcp23009::portPolarityMode(int value) {

    return gnublin_module_mcp230xx::portPolarityMode(0, value);
}


/**
 * @~english
 * @brief Read the interrupt capture for the port. This will clear
 * the interrupt on the port.
 *
 * @return The value read on the port at the time the interrupt occurs or -1 on error.
 */
unsigned char gnublin_module_mcp23009::readIntPort(void) {

    return gnublin_module_mcp230xx::readIntPort(0);
}


/**
 * @~english
 * @brief Read the interrupt flag for the port. This will return on which
 * pins interrupt occurs.
 *
 * @return The interrupt flags of the port or -1 on error.
 */
unsigned char gnublin_module_mcp23009::readIntFlagPort(void) {

    return gnublin_module_mcp230xx::readIntFlagPort(0);
}

/* -------------------------------------------------------------------------- */

// 
// module_mcp23009.cpp ends here
