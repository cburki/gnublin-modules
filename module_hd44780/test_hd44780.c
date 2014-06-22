/* test_hd44780.c --- 
 * 
 * Filename     : test_hd44780.c
 * Description  : Program for testing the HD44780 LCD module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sun May  4 14:30:36 2014
 * Version      : 1.0.0
 * Last-Updated : Sun Jun 22 11:40:30 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 55
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
#include "module_hd44780.h"

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the gnublin hd44780 module.\n");

    gnublin_hd44780_driver_gpio driverGpio(17, 18, 27, 22, 23, 24);
    gnublin_hd44780_driver_mcp23017 driverMcp23017;
    gnublin_module_hd44780 lcd(&driverMcp23017);

    lcd.init();
    if (lcd.fail()) {
        printf("error)%s\n", lcd.getErrorMessage());
    }

    sleep(2);
    lcd.print((char *)"BURKIONLINE.NET");
    if (lcd.fail()) {
        printf("error)%s\n", lcd.getErrorMessage());
    }

    sleep(2);
    lcd.controlDisplay(0, 1, 1);
    sleep(2);
    lcd.controlDisplay(1, 1, 1);

    lcd.print((char *)"Hello Word !", 2, 2);
    if (lcd.fail()) {
        printf("error)%s\n", lcd.getErrorMessage());
    }

    sleep(2);
    lcd.returnHome();
    sleep(2);
    lcd.controlDisplay(1, 0, 0);
    sleep(5);
    lcd.clear();
}

/* -------------------------------------------------------------------------- */

/* test_hd44780.c ends here */
