// module_sc16is740.cpp --- 
// 
// Filename     : module_sc16is740.cpp
// Description  : Class for accessing gnublin SC16IS740 single UART.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Thu May 29 15:14:01 2014
// Version      : 1.0.0
// Last-Updated : Sun Aug 31 16:12:27 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 856
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

#include "module_sc16is740.h"

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is740::gnublin_module_sc16is740(void)
    : gnublin_module_sc16is7x0() {

}


/**
 * @~english
 * @brief Set the given i2c address and the default i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is740::gnublin_module_sc16is740(int address)
    : gnublin_module_sc16is7x0(address) {

}


/**
 * @~english
 * @brief Set the given i2c address to 0x20 and the given i2c file to /dev/i2c-1.
 */
gnublin_module_sc16is740::gnublin_module_sc16is740(int address, std::string filename)
    : gnublin_module_sc16is7x0(address, filename) {

}

/* -------------------------------------------------------------------------- */

// 
// module_sc16is740.cpp ends here
