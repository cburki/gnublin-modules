/* module_sc16is740.h --- 
 * 
 * Filename     : module_sc16is740.h
 * Description  : Class for accessing gnublin SC16IS740 single UART.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Thu May 29 15:08:43 2014
 * Version      : 
 * Last-Updated : Sun Aug 31 16:12:16 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 119
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

#ifndef GNUBLIN_MODULE_SC16IS740
#define GNUBLIN_MODULE_SC16IS740

/* -------------------------------------------------------------------------- */

#include "gnublin.h"
#include "module_sc16is7x0.h"

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_module_sc16is740
 * @~english
 * @brief Class for accessing the SC16IS740 single uart via I2C.
 */
class gnublin_module_sc16is740 : public gnublin_module_sc16is7x0 {

 public :
    gnublin_module_sc16is740(void);
    gnublin_module_sc16is740(int address);
    gnublin_module_sc16is740(int address, std::string filename);
};

/* -------------------------------------------------------------------------- */

#endif

/* module_sc16is740.h ends here */
