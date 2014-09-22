// module_sht2x.cpp --- 
// 
// Filename     : module_sht2x.cpp
// Description  : Class for accessing gnublin STH2X temperature and humidity sensor.
// Author       : Christophe Burki
// Maintainer   : Christophe Burki
// Created      : Tue Sep 16 19:51:06 2014
// Version      : 1.0.0
// Last-Updated : Mon Sep 22 10:54:29 2014 (7200 CEST)
//           By : Christophe Burki
//     Update # : 114
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

#include "module_sht2x.h"

/* -------------------------------------------------------------------------- */

/* Define the commands. */
#define TEMP_HOLD   0xE3
#define HUM_HOLD    0xE5
#define TEMP_NOHOLD 0xF3
#define HUM_NOHOLD  0xF5
#define WRITE_USER  0xE6
#define READ_USER   0xE7
#define SOFT_RESET  0xFE

#define CRC_POLYNOMIAL 0x131  /* P(x) = x^8 + x^5 + x^4 + 1 = b100110001 */

/* -------------------------------------------------------------------------- */

/**
 * @~english
 * @brief Set the default i2c address to 0x40 and default i2c file to /dev/i2c-1.
 */
gnublin_module_sht2x::gnublin_module_sht2x(int address, std::string filename) {

    _errorFlag = false;

    setAddress(address);
    setDevicefile(filename);
}

/**
 * @~english
 * @brief Get the last error message.
 *
 * @return The error message as c-string.
 */
const char* gnublin_module_sht2x::getErrorMessage(void) {

    return _errorMessage.c_str();
}


/**
 * @~english
 * @brief Return whether the action fail or not.
 *
 * @return A boolean value indicating if the action fail or not.
 */
bool gnublin_module_sht2x::fail(void) {

    return _errorFlag;
}


/**
 * @~english
 * @brief Set the i2c address.
 *
 * @param address The address to set.
 */
void gnublin_module_sht2x::setAddress(int address) {

    _i2c.setAddress(address);
}


/**
 * @~english
 * @brief Set the i2c device file.
 *
 * @param filename The i2c device filename.
 */
void gnublin_module_sht2x::setDevicefile(std::string filename) {

    _i2c.setDevicefile(filename);
}


/**
 * @~english
 * @brief Perform a reset.
 *
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sht2x::softReset(void) {

    _errorFlag = false;
    unsigned char txBuffer = SOFT_RESET;

    if (_i2c.send(&txBuffer, 1) > 0) {
        return 1;
    }

    _errorFlag = true;
    _errorMessage = "i2c.send (SOFT_RESET) Error\n";
    return -1;
}


/**
 * @~english
 * @brief Read the temperature from the sensor.
 *
 * @return -99.99 on error and the temperature on success.
 */
float gnublin_module_sht2x::readTemperature(void) {

    _errorFlag = false;
    float temperature = -99.99;
    unsigned char rxBuffer[3];

    /* Read the temperature. */
    if (_readSensor(rxBuffer, TEMP_NOHOLD) < 0) {
        return temperature;
    }

    /* Check the CRC. */
    if (_checkCrc(rxBuffer, 2, rxBuffer[2]) < 0) {
        return -99.99;
    }

    unsigned int result = (rxBuffer[0] << 8) + rxBuffer[1];
    result &= 0xFFFC;  /* Clear the status bit */
    temperature = -46.65 + 175.72 / 65536.0 * result;
    return temperature;
}


/**
 * @~english
 * @brief Read the relative humidity from the sensor.
 *
 * @return -99.99 on error and the humidity on success.
 */
float gnublin_module_sht2x::readHumidity(void) {

    _errorFlag = false;
    float humidity = -99.99;
    unsigned char rxBuffer[3];

    /* Read the temperature. */
    if (_readSensor(rxBuffer, HUM_NOHOLD) < 0) {
        return humidity;
    }

    /* Check the CRC. */
    if (_checkCrc(rxBuffer, 2, rxBuffer[2]) < 0) {
        return -99.99;
    }

    unsigned int result = (rxBuffer[0] << 8) + rxBuffer[1];
    result &= 0xFFFC;  /* Clear the status bit */
    humidity = -6.0 + 125.0 / 65536.0 * result;
    return humidity;
}


/**
 * @~english
 * @brief Convert ceclius to farenheit.
 *
 * @param c The value in celcius.
 * @return The converted value in farenheit.
 */
float gnublin_module_sht2x::convertC2F(float c) {

    return c * 9 / 5 + 32;
}


/**
 * @~english
 * @brief Read the value for the given command.
 *
 * @param data The buffer where will be stored the read data.
 * @param command The command specifying what to read fro sensor.
 * @return -1 on error and 1 on success.
 */
int gnublin_module_sht2x::_readSensor(unsigned char *data, unsigned char command) {

    _errorFlag = false;

    /* Start measurement. */
    if (_i2c.send(&command, 1) < 0) {
        _errorFlag = true;
        sprintf(const_cast<char*>(_errorMessage.c_str()), "i2c.send (0x%02x) Error\n", command);
        return -1;
    }

    usleep(1000 * 100);

    /* Read data. */
    if (_i2c.receive(data, 3) < 0) {
        _errorFlag = true;
        sprintf(const_cast<char*>(_errorMessage.c_str()), "i2c.receive (0x%02x) Error\n", command);
        return -1;
    }

    //printf("MSB=0x%02x LSB=0x%02x CSUM=0x%02x\n", data[0], data[1], data[2]);
    return 1;
}


/**
 * @~english
 * @brief Calculates checksum for n bytes of data and compares it with expected checksum.
 *
 * @param data The data to which to compute the checksum.
 * @param bytes The number of bytes to compute the checksum.
 * @param checksum The checksum to compare the computed checksum.
 */
int gnublin_module_sht2x::_checkCrc(unsigned char *data, int bytes, unsigned char checksum) {

    _errorFlag = false;
    unsigned char crc = 0;

    for (int i = 0; i < bytes; i++) {
        crc ^= data[i];
        for (int bit = 8; bit > 0; bit--) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            }
            else {
                crc = (crc << 1);
            }
        }
    }

    if (crc != checksum) {
        _errorFlag = true;
        _errorMessage = "CRC error !";
        return -1;
    }

    return 0;
}

/* -------------------------------------------------------------------------- */

// 
// module_sht2x.cpp ends here
