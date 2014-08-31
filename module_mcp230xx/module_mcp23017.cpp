// module_mcp23017.cpp --- 
// 
// Filename     : module_mcp23017.cpp
// Description  : Class for accessing gnublin mcp23017 port expander.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Sun Apr 27 15:39:20 2014
// Version      : 1.0.0
// Last-Updated : Fri Aug 29 16:41:01 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 521
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

#include "module_mcp23017.h"

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x20 and default i2c file to /dev/i2c-1.
 */
gnublin_module_mcp23017::gnublin_module_mcp23017(void)
    : gnublin_module_mcp230xx(2, 16) {

}

/* -------------------------------------------------------------------------- */

// 
// module_mcp23017.cpp ends here
