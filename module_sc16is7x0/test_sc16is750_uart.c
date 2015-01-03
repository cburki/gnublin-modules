/* test_sc16is750_uart.c --- 
 * 
 * Filename     : test_sc16is750_uart.c
 * Description  : Test the UART of the sc16is750 module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Thu May 29 19:18:54 2014
 * Version      : 1.0.0
 * Last-Updated : Sat Jan  3 09:21:24 2015 (3600 CET)
 *           By : Christophe Burki
 *     Update # : 144
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
#include <poll.h>

#include "gnublin.h"
#include "module_sc16is750.h"

/* -------------------------------------------------------------------------- */

#define POLL_TIMEOUT (1 * 1000)  /* 1 second */

/* -------------------------------------------------------------------------- */

void onDataReceived(char *buffer, int len) {
    printf("onDataReceived(buffer=%s, len=%d)\n", buffer, len);

    free(buffer);
}


void onSpaceAvailable(int len) {
    printf("onSpaceAvailable(len=%d)\n", len);
}

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the UART of the gnublin sc16is750 module.\n");

    gnublin_gpio gpio;
    gnublin_module_sc16is750 sc16is750(0x49);

    sc16is750.init();
    sc16is750.enableFifo(1);

    sc16is750.rxEmptyFifo();
    int space = sc16is750.txAvailableSpace();
    int bytes = 0;
    printf("space=%d\n", space);

    sc16is750.writeByte('H');
    sc16is750.writeByte('e');
    sc16is750.writeByte('l');
    sc16is750.writeByte('l');
    sc16is750.writeByte('o');
    sc16is750.writeByte(' ');
    bytes = sc16is750.write("World\r\n", 7);
    printf("bytes=%d\n", bytes);
    bytes = sc16is750.write("0123456789012345678901234567890123456789012345678901234567890123567890123456789\n", 81);
    printf("bytes=%d\n", bytes);

    int available = 0;
    char buffer[2048];
    int i = 0;
    while (1) {

        if ((available = sc16is750.rxAvailableData()) > 0) {

            char rcvBuf[available + 1];
            sc16is750.read(rcvBuf, available);
            if (rcvBuf[available - 1] == 0x0d) {
                buffer[i] = '\0';
                i = 0;
            }
            else {
                rcvBuf[available] = '\0';
                buffer[i] = *rcvBuf;
                i += available;
            }

            printf("buffer=%s\n", buffer);

            if (strcmp(buffer, "exit") == 0) {
                break;
            }
        }
    }

    /*
     * Testing UART interrupts
     *
     * R-PI    SC16IS750
     * #22     IRQ
     * #18     GPIO0
     *
     * Press 'q' to exit.
     */

    gpio.pinMode(18, OUTPUT);
    gpio.pinMode(22, INPUT);

    int nfds = 2;
    struct pollfd fdset[nfds];
    int gpio22Fd;
    int gpio22EdgeFd;
    int timeout = POLL_TIMEOUT;
    int nfd;

    gpio22EdgeFd = open("/sys/class/gpio/gpio22/edge", O_WRONLY);
    if (gpio22EdgeFd < 0) {
        printf("gpio22EdgeFd Error");
        return -1;
    }
    write(gpio22EdgeFd, "falling", 7);
    close(gpio22EdgeFd);

    gpio22Fd = open("/sys/class/gpio/gpio22/value", O_RDONLY | O_NONBLOCK);
    if (gpio22Fd < 0) {
        printf("gpio22Fd/open Error");
        return -1;
    }

    /* */
    read(gpio22Fd, NULL, 1);

    /* Initialize the pollfd structure. */
    memset((void *)fdset, 0, sizeof(fdset));
    fdset[0].fd = STDIN_FILENO;
    fdset[0].events = POLLIN;
    fdset[1].fd = gpio22Fd;
    fdset[1].events = POLLPRI;


    sc16is750.intIsrDataReceived(&onDataReceived);
    sc16is750.intIsrSpaceAvailable(&onSpaceAvailable);
    
    sc16is750.init();
    if (sc16is750.fail()) {
        printf("ERROR : %s\n", sc16is750.getErrorMessage());
    }

    sc16is750.enableFifo(1);
    sc16is750.rxFifoSetTriggerLevel(8);
    sc16is750.txFifoSetTriggerLevel(8);
    sc16is750.setInterrupt(CONF_INT_RHREN | CONF_INT_THREN);
    sc16is750.rxEmptyFifo();

    unsigned char status = sc16is750.readLineStatus();
    printf("status=0x%02x\n", status);

    while (1) {
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

            if (*c == 'q') {
                break;
            }
        }

        if (fdset[1].revents && POLLPRI) {
            read(fdset[1].fd, NULL, 1);
            int intCount = sc16is750.pollInt();
            printf("intCount=%d\n", intCount);
        }
    }

    close(gpio22Fd);
}

/* -------------------------------------------------------------------------- */

/* test_sc16is750_uart.c ends here */
