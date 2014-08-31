/* test_sc16is750.c --- 
 * 
 * Filename     : test_sc16is750.c
 * Description  : Test the sc16is750 module.
 * Author       : Christophe Burki
 * Maintainer   : Christophe Burki
 * Created      : Thu May 29 19:18:54 2014
 * Version      : 1.0.0
 * Last-Updated : Sun Aug 31 17:25:59 2014 (7200 CEST)
 *           By : Christophe Burki
 *     Update # : 282
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


void isrIO(int pin, int value) {
    printf("isrIO(pin=%d, value=%d)\n", pin, value);
}

/* -------------------------------------------------------------------------- */

int main(void) {
    printf("Testing the gnublin sc16is750 module.\n");

    gnublin_gpio gpio;
    gnublin_module_sc16is750 sc16is750(0x4d);
 
    sc16is750.init();
    sc16is750.initUART(CONF_INT_RHREN);
    sc16is750.initIO(CONF_IO_LATCH);

    /* Test the GPIOs */
    sc16is750.portMode(OUTPUT);
    if (sc16is750.fail()) {
        printf("ERROR : %s\n", sc16is750.getErrorMessage());
    }

    sc16is750.digitalWrite(1, HIGH);
    if (sc16is750.fail()) {
        printf("ERROR : %s\n", sc16is750.getErrorMessage());
    }

    usleep(1000 * 1000);

    sc16is750.digitalWrite(1, LOW);
    if (sc16is750.fail()) {
        printf("ERROR : %s\n", sc16is750.getErrorMessage());
    }

    sc16is750.pinMode(0, INPUT);
    if (sc16is750.fail()) {
        printf("ERROR : %s\n", sc16is750.getErrorMessage());
    }

    unsigned char val[1];
    val[0] = sc16is750.readPort();
    printf("val=0x%02x\n", val[0]);


    /*
     * Testing GPIOs interrupts
     *
     * R-PI    SC16IS750
     * #23     IRQ
     * #18     GPIO0
     *
     * Press character 'h' to put the pin #18 to HIGH and 
     * press 'l' to put it LOW. 'q' to exit. A change in
     * the pin #18 state fire an interrupt.
     */

    gpio.pinMode(18, OUTPUT);
    gpio.pinMode(23, INPUT);
    sc16is750.pinIntEnable(0, 1);
    sc16is750.intIsrDataReceived(&onDataReceived);
    sc16is750.intIsrSpaceAvailable(&onSpaceAvailable);
    sc16is750.intIsrIO(&isrIO);

    sc16is750.setBaudRate(UART_9600);
    sc16is750.rxFifoSetTriggerLevel(8);
    sc16is750.txFifoSetTriggerLevel(8);
    sc16is750.rxEmptyFifo();

    int nfds = 2;
    struct pollfd fdset[nfds];
    int gpio23Fd;
    int gpio23EdgeFd;
    int timeout = POLL_TIMEOUT;
    int nfd;

    gpio23EdgeFd = open("/sys/class/gpio/gpio23/edge", O_WRONLY);
    if (gpio23EdgeFd < 0) {
        printf("gpio23EdgeFd Error");
        return -1;
    }
    write(gpio23EdgeFd, "falling", 7);
    close(gpio23EdgeFd);

    gpio23Fd = open("/sys/class/gpio/gpio23/value", O_RDONLY | O_NONBLOCK);
    if (gpio23Fd < 0) {
        printf("gpio23Fd/open Error");
        return -1;
    }

    /* Initialize the pollfd structure. */
    memset((void *)fdset, 0, sizeof(fdset));
    fdset[0].fd = STDIN_FILENO;
    fdset[0].events = POLLIN;
    fdset[1].fd = gpio23Fd;
    fdset[1].events = POLLPRI;

    /* */
    read(gpio23Fd, NULL, 1);

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

            if (*c == 'h') {
                gpio.digitalWrite(18, HIGH);
            }
            if (*c == 'l') {
                gpio.digitalWrite(18, LOW);
            }
            if (*c == 'q') {
                break;
            }
        }

        if (fdset[1].revents && POLLPRI) {
            unsigned char readBuffer[64];
            int bytesRead = read(fdset[1].fd, readBuffer, 64);
            int intCount = sc16is750.pollInt();
            printf("intCount=%d\n", intCount);
        }
    }

    close(gpio23Fd);


    /* Testing the UART.
     * Send "Hello " character after character,
     * Send "Word\n" as a string,
     * Send "0123...89\n" a 81 characters string (greater than the FIFO size),
     * Wait for string "exit" and exit.
     */
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
    bytes = sc16is750.write("Word\r\n", 6);
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
            
            if (strcmp(buffer, "exit") == 0) {
                break;
            }
        }
    }
}

/* -------------------------------------------------------------------------- */

/* test_sc16is750.c ends here */
