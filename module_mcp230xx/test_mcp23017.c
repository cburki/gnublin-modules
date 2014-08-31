/* test_mcp23017.c --- 
 * 
 * Filename     : test_mcp23017.c
 * Description  : Test the mcp23017 module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sun Apr 27 16:00:55 2014
 * Version      : 1.0.0
 * Last-Updated : Sat Aug 30 19:41:25 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 246
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
#include "module_mcp23017.h"

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the gnublin mcp23017 module.\n");

    gnublin_module_mcp23017 mcp23017;

    mcp23017.portMode(0, OUTPUT);
    mcp23017.portMode(1, INPUT);

    for (unsigned int i = 0; i <= 7; i++) {
        printf("pin=%d\n", i);
        int result = mcp23017.digitalWrite(i, HIGH);
        if (result < 0) {
            printf("error=%s\n", mcp23017.getErrorMessage());
        }
        usleep(500 * 1000);
    }

    for (int i = 7; i >= 0; i--) {
        printf("pin=%d\n", i);
        int result = mcp23017.digitalWrite(i, LOW);
        if (result < 0) {
            printf("error=%s\n", mcp23017.getErrorMessage());
        }
        usleep(500 * 1000);
    }

    mcp23017.writePort(0, 0xAA);
    usleep(1000 * 1000);
    mcp23017.writePort(0, 0x55);
    usleep(1000 * 1000);
    mcp23017.writePort(0, 0x00);

    int pin8 = mcp23017.digitalRead(8);
    printf("pin8=%d\n", pin8);
    if (pin8 == -1) {
        printf("error=%s\n", mcp23017.getErrorMessage());
    }

    unsigned char buffer;
    buffer = mcp23017.readPort(1);
    printf("buffer=0x%02x\n", buffer);
}

/* -------------------------------------------------------------------------- */

/* test_mcp23017.c ends here */
