/* test_sht2x.c --- 
 * 
 * Filename     : test_sht2x.c
 * Description  : Test the SHT2X module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Tue Sep 16 19:54:50 2014
 * Version      : 1.0.0
 * Last-Updated : Sun Sep 21 16:27:17 2014 (7200 CEST)
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

/* -------------------------------------------------------------------------- */

#include <stdio.h>

#include "gnublin.h"
#include "module_sht2x.h"

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the gnublin sht2x module.\n");

    gnublin_module_sht2x sht21;
    sht21.softReset();
    if (sht21.fail()) {
        printf(sht21.getErrorMessage());
        return 1;
    }
    usleep(1000 * 50);

    float temperature = sht21.readTemperature();
    if (sht21.fail()) {
        printf(sht21.getErrorMessage());
        return 1;
    }
    printf("temperature=%0.2f\n", temperature);

    usleep(1000 * 50);

    float humidity = sht21.readHumidity();
    if (sht21.fail()) {
        printf(sht21.getErrorMessage());
        return 1;
    }
    printf("humidity=%0.2f\n", humidity);

    return 0;
}

/* -------------------------------------------------------------------------- */

/* test_sht2x.c ends here */
