/* module_mcp23009.h --- 
 * 
 * Filename     : module_mcp23009.h
 * Description  : Class for accessing gnublin MCP23009 port expander.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sun Apr 27 15:27:32 2014
 * Version      : 1.0.0
 * Last-Updated : Sat Aug 30 19:29:02 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 7
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

#ifndef GNUBLIN_MODULE_MCP23009
#define GNUBLIN_MODULE_MCP23009

/* -------------------------------------------------------------------------- */

#include "gnublin.h"
#include "module_mcp230xx.h"

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_module_mcp23009
 * @~english
 * @brief Class for accessing the MCP23017 and MCP23009 port expander via I2C.
 */
class gnublin_module_mcp23009 : public gnublin_module_mcp230xx {

 public :
    gnublin_module_mcp23009(void);
    int portMode(std::string direction);
    int writePort(unsigned char value);
    unsigned char readPort(void);

    int portIntMode(std::string mode);
    int portPullUpMode(int value);
    int portPolarityMode(int value);
    unsigned char readIntPort(void);
    unsigned char readIntFlagPort(void);
};

/* -------------------------------------------------------------------------- */

#endif

/* module_mcp23009.h ends here */
