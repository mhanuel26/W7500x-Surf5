/*============================================================================
* Wiznet WebServer (Surf5) Super-Simple Tasker (SST/C) Example
*
* Copyright (C) 2024 Manuel Iglesias
*
* SPDX-License-Identifier: MIT
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
============================================================================*/
#include "sst.h"           /* SST framework */
#include "bsp.h"           /* Board Support Package interface */
#include "webserver.h"           /* Board Support Package interface */
#include "wizchip_conf.h"
#include "w7500x.h"
#include <stdio.h>
#include <string.h>
#include <socket.h>


DBC_MODULE_NAME("webserver")  /* for DBC assertions in this module */

/* Blinky event-driven task ------------------------------------------------*/
typedef struct {
    SST_Task super; /* inherit SST_Task */

    SST_TimeEvt te1;
    volatile uint8_t phy_rdy;
} Server;

extern uint8_t test_buf[DATA_BUF_SIZE];

static void Webserver_ctor(Server * const me);
static void Webserver_init(Server * const me, SST_Evt const * const ie);
static void Webserver_dispatch(Server * const me, SST_Evt const * const e);
static int32_t WebServer(uint8_t sn, uint8_t* buf, uint16_t port);

/*..........................................................................*/
static Server Webserver_inst; /* the Blinky instance */
SST_Task * const AO_Server = &Webserver_inst.super; /* opaque AO pointer */

bool Webserver_get_phyready(void){
    bool res;
    SST_LockKey key = SST_Task_lock(3U);
    res = (Webserver_inst.phy_rdy == TRUE);
    SST_Task_unlock(key);
    return res;
}

void Webserver_set_phyready(void){
    SST_LockKey key = SST_Task_lock(3U);
    Webserver_inst.phy_rdy = TRUE;
    SST_Task_unlock(key);
}
/*..........................................................................*/
void Webserver_instantiate(void) {
    Webserver_inst.phy_rdy = FALSE;
    Webserver_ctor(&Webserver_inst);
}
/*..........................................................................*/
void Webserver_ctor(Server * const me) {
    SST_Task_ctor(
       &me->super,
       (SST_Handler)&Webserver_init,
       (SST_Handler)&Webserver_dispatch);
    SST_TimeEvt_ctor(&me->te1, GET_SN_SR_SIG, &me->super);
}


/* Server implementation with one periodic time event */

static void Webserver_init(Server * const me, SST_Evt const * const ie) {
    (void)ie; /* Server instance parameter */
    SST_TimeEvt_arm(&me->te1, 1U, 0U);
}
/*..........................................................................*/
static void Webserver_dispatch(Server * const me, SST_Evt const * const e) {
    switch (e->sig) {
        case GET_SN_SR_SIG: {
            BSP_a0on();
            if(Webserver_get_phyready()) {
                WebServer(1, test_buf, 80);
            }
            SST_TimeEvt_arm(&me->te1, BSP_TICKS_PER_SEC * 1U/1000U, 0U);
            BSP_a0off();
            break;
        }
        default: {
            DBC_ERROR(200);
            break;
        }
    }
}

/**
 * @brief  WebServer example function.
 * @note
 * @param  sn: Socket number to use.
 * @param  buf: The buffer the socket will use.
 * @param  port: Socket port number to use.
 * @retval Success or Fail of configuration functions
 */
static int32_t WebServer(uint8_t sn, uint8_t* buf, uint16_t port)
{
    int32_t ret;
    uint16_t size = 0;
    uint8_t destip[4];
    uint16_t destport;
    uint8_t adc_buf[128] = { '\0', };

    switch (getSn_SR(sn))
    {
        case SOCK_ESTABLISHED:

            if (getSn_IR(sn) & Sn_IR_CON) {

                getSn_DIPR(sn, destip);
                destport = getSn_DPORT(sn);
                printf("%d:Connected - %d.%d.%d.%d : %d\r\n", sn, destip[0], destip[1], destip[2], destip[3], destport);

                setSn_IR(sn, Sn_IR_CON);
            }

            if ((size = getSn_RX_RSR(sn)) > 0) {
                if (size > DATA_BUF_SIZE) size = DATA_BUF_SIZE;
                ret = recv(sn, buf, size);
                if (ret <= 0) return ret;
                printf("%s", buf);

                ret = send(sn, (uint8_t*)"HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "Refresh: 5\r\n"
                        "\r\n"
                        "<!DOCTYPE HTML>\r\n"
                        "<html>\r\n", sizeof("HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Connection: close\r\n"
                        "Refresh: 5\r\n"
                        "\r\n"
                        "<!DOCTYPE HTML>\r\n"
                        "<html>\r\n") - 1);
                if (ret < 0) {
                    close(sn);
                    return ret;
                }

                sprintf((char*)adc_buf, (const char*)"HELLO WIZNET SST WORLD!<br />\r\n");
                ret = send(sn, adc_buf, strlen((const char*)adc_buf));
                if (ret < 0) {
                    close(sn);
                    return ret;
                }

                ret = send(sn, (uint8_t*)"</html>\r\n", sizeof("</html>\r\n") - 1);
                if (ret < 0) {
                    close(sn);
                    return ret;
                }

                disconnect(sn);
            }

            break;
        case SOCK_CLOSE_WAIT:

            if ((ret = disconnect(sn)) != SOCK_OK) return ret;

            printf("%d:Socket Closed\r\n", sn);

            break;
        case SOCK_INIT:

            printf("%d:Listen, Web server, port [%d]\r\n", sn, port);

            if ((ret = listen(sn)) != SOCK_OK) return ret;

            break;
        case SOCK_CLOSED:

            if ((ret = socket(sn, Sn_MR_TCP, port, 0x00)) != sn) return ret;

            break;
        default:
            break;
    }
    return 1;
}