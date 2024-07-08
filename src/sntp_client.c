/*============================================================================
* Wiznet SNTP Client (Surf5) Super-Simple Tasker (SST/C) Example
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
#include "server.h"           /* Board Support Package interface */
#include "wizchip_conf.h"
#include "w7500x.h"
#include <stdio.h>
#include <string.h>
#include <socket.h>
#include "sntp.h"
#include "led_matrix.h"

DBC_MODULE_NAME("sntpclient")  /* for DBC assertions in this module */


/* Timeout */
#define RUN_TICKS   100U
#define RECV_TIMEOUT (RUN_TICKS * 10)     // 10ms tick under RUN state10 seconds

/* Blinky event-driven task ------------------------------------------------*/
typedef struct {
    SST_Task super; /* inherit SST_Task */

    SST_TimeEvt te1;
    SST_TimeEvt te2;

    uint32_t sectick;
} Client;

extern uint8_t srv_buf[DATA_BUF_SIZE];
extern datetime Nowdatetime;

static void Sntpserver_ctor(Client * const me);
static void Sntpserver_init(Client * const me, SST_Evt const * const ie);
static void Sntpserver_dispatch(Client * const me, SST_Evt const * const e);
static uint8_t  inc_sec_ticks(void);
static uint32_t get_sec_ticks(void);
datetime time;
static MatrixWorkEvt fInitDoneEvt = {
    .super.sig = USER_ONE_SHOT,
    .text = "",
    .scroll_iter = 0        // 0 here for one shot image send.
};
static bool toogle_colon = TRUE;
static bool volatile sec_upd_pause = true;
#ifdef FONT_SIZE_6X8
static char const fmt_colon_on[] = {"%02d:%02d"};
static char const fmt_colon_off[] = {"%02d %02d"};
#else
static char const fmt_colon_on[] = {"%02d:%02d:%02d"};
static char const fmt_colon_off[] = {"%02d:%02d:%02d"};
#endif
/*..........................................................................*/
static Client Sntpserver_inst; /* the Blinky instance */
SST_Task * const AO_Sntp = &Sntpserver_inst.super; /* opaque AO pointer */


static void copy_datetime_object(datetime* time){
    time->yy = Nowdatetime.yy;
    time->mo = Nowdatetime.mo;
    time->dd = Nowdatetime.dd;
    time->hh = Nowdatetime.hh;
    time->mm = Nowdatetime.mm;
    time->ss = Nowdatetime.ss;
}

static uint8_t  inc_sec_ticks(void){
    Sntpserver_inst.sectick += 1;
    if(Sntpserver_inst.sectick >= SNTP_REFRESH){
        Sntpserver_inst.sectick = 0;
        return TRUE;
    }
    return FALSE;
} 

static uint32_t get_sec_ticks(void){
    return Sntpserver_inst.sectick;
} 

/*..........................................................................*/
void Sntpserver_instantiate(void) {
    Sntpserver_inst.sectick = 0;
    Sntpserver_ctor(&Sntpserver_inst);
}
/*..........................................................................*/
void Sntpserver_ctor(Client * const me) {
    SST_Task_ctor(
       &me->super,
       (SST_Handler)&Sntpserver_init,
       (SST_Handler)&Sntpserver_dispatch);
    SST_TimeEvt_ctor(&me->te1, GET_SN_SR_SIG, &me->super);
    SST_TimeEvt_ctor(&me->te2, SNTP_RUN, &me->super);
}

/* Client implementation with one periodic time event */
static void Sntpserver_init(Client * const me, SST_Evt const * const ie) {
    (void)ie; /* Client instance parameter */
    // SST_TimeEvt_arm(&me->te1, BSP_TICKS_PER_SEC * 1U, 0U);
}
/*..........................................................................*/
static void Sntpserver_dispatch(Client * const me, SST_Evt const * const e) {
    switch (e->sig) {
        case GET_SN_SR_SIG: {
#if SNTP_DBG_BLIP
            BSP_c0on();
#endif
            if(inc_sec_ticks()) {
                /* Setup for Run SNTP request task */
                static SntpWorkEvt const PhyInitDoneEvt = {
                    .super.sig = SNTP_RUN,
                };
                /* Enqueue RUN event in SNTP Task */
                SST_Task_post(AO_Sntp, &PhyInitDoneEvt.super);
            }else{
                /* Update here our RTC internal clock values */
                tstamp tmp = changedatetime_to_seconds() + 1;
                calcdatetime(tmp);
                copy_datetime_object(&time);               
#ifdef FONT_SIZE_6X8
                if(sec_upd_pause){
                    memset(fInitDoneEvt.text, 0, sizeof(fInitDoneEvt.text));
                    if(toogle_colon){
                        sprintf(fInitDoneEvt.text, fmt_colon_on, time.hh, time.mm);
                    }else{
                        sprintf(fInitDoneEvt.text, fmt_colon_off, time.hh, time.mm);
                    }
                    toogle_colon ^= TRUE;
                    SST_Task_post(AO_Matrix, &fInitDoneEvt.super);
                }
#endif
                // printf("Software date-time:%d/%02d/%d-%02d:%02d:%02d\n\r", time.dd, time.mo, time.yy, time.hh, time.mm, time.ss);
                SST_TimeEvt_arm(&me->te1, BSP_TICKS_PER_SEC * 1U, 0U);      /* update clock every second */
            }
#if SNTP_DBG_BLIP
            BSP_c0off();
#endif
            break;
        }
        case SNTP_RUN: {
#if SNTP_DBG_BLIP
            BSP_c0on();
#endif
            int8_t retval = 0;
            retval = SNTP_run(&time);
            if (retval == 1)
            {
                printf("SNTP date-time: %d/%02d/%d-%02d:%02d:%02d \n\r", time.dd, time.mo, time.yy, time.hh, time.mm, time.ss);
                SST_TimeEvt_arm(&me->te1, BSP_TICKS_PER_SEC * 1U, 0U);      /* update clock every second */
            }else if(retval == 0){
                SST_TimeEvt_arm(&me->te2, 10U, 0U);      /* call the SNTP RUN Task every 10 ms */
            }else if(retval == -1){
                printf("SNTP failed to get request response.\n\r");
            }
#if SNTP_DBG_BLIP
            BSP_c0off();
#endif
            break;
        }
        case SNTP_PAUSE: {
            sec_upd_pause ^= true;
            break;
        }
        default: {
            DBC_ERROR(200);
            break;
        }
    }
}

