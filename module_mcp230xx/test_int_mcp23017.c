/* test_int_mcp23017.c --- 
 * 
 * Filename     : test_int_mcp23017.c
 * Description  : Test the interrupt for the mcp23017 module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Sun Apr 27 16:00:55 2014
 * Version      : 1.0.0
 * Last-Updated : Sun Jun 22 11:43:56 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 248
 * URL          : 
 * Keywords     : 
 * Compatibility: 
 * 
 */

/* Commentary   : This program test and show the interrupt handling with
 *                a MCP23017 port expander and a RaspberryPi.
 *
 *                R-PI         MCP23017
 *                #24    <->   INTB
 *                #23    <->   GPB0
 * 
 *                When a rising edge occurs on pin #23, an interrupt
 *                occurs (MCP23017) and the pin INTB go HIGH. This is
 *                detected by a poll on the pin #24 file descriptor.
 *                The isr function is called when an interrupt occurs
 *                on the MCP23017.
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
#include <poll.h>

#include "gnublin.h"
#include "module_mcp23017.h"

/* -------------------------------------------------------------------------- */

#define POLL_TIMEOUT (1 * 1000)  /* 1 second */

/* -------------------------------------------------------------------------- */

void isr(int port, int pin, int value) {
    printf("isr(port=%d, pin=%d, value=%d)\n", port, pin, value);
}

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the gnublin mcp23017 module.\n");
    printf("Press 'i' to fire an interrupt and 'q' to quit.\n");

    gnublin_gpio gpio;
    gnublin_module_mcp23017 mcp23017;

    gpio.pinMode(23, OUTPUT);
    gpio.pinMode(24, INPUT);
    mcp23017.init(CONF_INTHIGH);
    mcp23017.portMode(0, OUTPUT);
    mcp23017.portMode(1, INPUT);

    /* Enabling interrupts on pin 8. */
    mcp23017.pinIntMode(8, INT_CHANGE);
    if (mcp23017.fail()) {
        printf("error=%s\n", mcp23017.getErrorMessage());
    }
    mcp23017.intIsr(&isr);

    int nfds = 2;
    struct pollfd fdset[nfds];
    int gpio24Fd;
    int gpio24EdgeFd;
    int timeout = POLL_TIMEOUT;
    int nfd;

    gpio24EdgeFd = open("/sys/class/gpio/gpio24/edge", O_WRONLY);
    if (gpio24EdgeFd < 0) {
        printf("gpio24EdgeFd Error");
        return -1;
    }
    write(gpio24EdgeFd, "rising", 7);
    close(gpio24EdgeFd);

    gpio24Fd = open("/sys/class/gpio/gpio24/value", O_RDONLY | O_NONBLOCK);
    if (gpio24Fd < 0) {
        printf("gpio24Fd/open Error");
        return -1;
    }

    /* Initialize the pollfd structure. */
    memset((void *)fdset, 0, sizeof(fdset));
    fdset[0].fd = STDIN_FILENO;
    fdset[0].events = POLLIN;
    fdset[1].fd = gpio24Fd;
    fdset[1].events = POLLPRI;

    /* */
    read(gpio24Fd, NULL, 1);

    while (1) {
        gpio.digitalWrite(23, LOW);

        nfd = poll(fdset, nfds, timeout);
        if (nfd < 0) {
            printf("poll() failed\n");
            return -1;
        }

        if (nfd == 0) {
            /* Poll timeout. */
        }

        if (fdset[0].revents & POLLIN) {
            char *c = (char *)malloc(sizeof(char));
            read(fdset[0].fd, c, 1);

            if (*c == 'i') {
                gpio.digitalWrite(23, HIGH);
            }
            if (*c == 'q') {
                break;
            }
        }

        if (fdset[1].revents && POLLPRI) {
            unsigned char readBuffer[64];
            int bytesRead = read(fdset[1].fd, readBuffer, 64);
            int intCount = mcp23017.pollInt();
        }
    }

    close(gpio24Fd);
}

/* -------------------------------------------------------------------------- */

/* test_int_mcp23017.c ends here */
