/* module_sht2x.h --- 
 * 
 * Filename     : module_sht2x.h
 * Description  : Class for accessing gnublin SHT2X temperature and humidity sensor.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Mon Sep 15 21:00:07 2014
 * Version      : 1.0.0
 * Last-Updated : Sun Oct 12 14:59:47 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 20
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

#ifndef GNUBLIN_MODULE_SHT2X
#define GNUBLIN_MODULE_SHT2X

/* -------------------------------------------------------------------------- */

#include "gnublin.h"

/* -------------------------------------------------------------------------- */

/**
 * @class gnublin_module_sht2x
 * @~english
 * @brief Class for accessing the SHT2X temperature and humidity sensor via I2C.
 */
class gnublin_module_sht2x {

  private :
    gnublin_i2c i2c;
    bool errorFlag;
    std::string errorMessage;

    int readSensor(unsigned char *data, unsigned char command);
    int checkCrc(unsigned char *data, int bytes, unsigned char checksum);

 public :
    gnublin_module_sht2x(int address = 0x40, std::string filename = "/dev/i2c-1");
    const char* getErrorMessage(void);
    bool fail(void);
    void setAddress(int address);
    void setDevicefile(std::string filename);
    int softReset(void);
    float readTemperature(void);
    float readHumidity(void);
    float convertC2F(float c);
};

/* -------------------------------------------------------------------------- */

#endif

/* module_sht21.h ends here */
