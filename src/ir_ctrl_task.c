/*============================================================================
* Super-Simple Tasker (SST/C) Example
*
* Copyright (C) 2024, Manuel Iglesias
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
#include <stdio.h>
#include "ir_ctrl_task.h"
#include "nec_decoder.h"
#include "server.h"        /* for the SNTP task events */

DBC_MODULE_NAME("ir_ctrl_task")  /* for DBC assertions in this module */

/* IrCtrl event-driven task ------------------------------------------------*/
typedef struct {
    SST_Task super; /* inherit SST_Task */

    SST_TimeEvt te1;
} IrCtrl;

static void IrCtrl_ctor(IrCtrl * const me);
static void IrCtrl_init(IrCtrl * const me, SST_Evt const * const ie);
static void IrCtrl_dispatch(IrCtrl * const me, SST_Evt const * const e);

/* Static Event declarations */
static SntpWorkEvt const SntpPauseCmd = {
    .super.sig = SNTP_PAUSE,
    
};

/*..........................................................................*/
static IrCtrl IrCtrl_inst; /* the IrCtrl instance */
SST_Task * const AO_IrCtrl = &IrCtrl_inst.super; /* opaque AO pointer */
static volatile uint32_t code = 0x0;
/*..........................................................................*/
void IrCtrl_instantiate(void) {
    IrCtrl_ctor(&IrCtrl_inst);
}
/*..........................................................................*/
void IrCtrl_ctor(IrCtrl * const me) {
    SST_Task_ctor(
       &me->super,
       (SST_Handler)&IrCtrl_init,
       (SST_Handler)&IrCtrl_dispatch);
    SST_TimeEvt_ctor(&me->te1, CHECK_RECEPTION, &me->super);
}

static void IrCtrl_init(IrCtrl * const me, SST_Evt const * const ie) {
    (void)ie; /* unused parameter */
    SST_TimeEvt_arm(&me->te1, 1U, 0U);
}
/*..........................................................................*/
static void IrCtrl_dispatch(IrCtrl * const me, SST_Evt const * const e) {
    switch (e->sig) {
        case CHECK_RECEPTION: {
            if(available() == true){
                code = readPacket();
                switch(code){
                    case BUTTON_SHARP:
                        /* Send event to SNTP task to stop refreshing display every second. */
                        SST_Task_post(AO_Sntp, &SntpPauseCmd.super);
                    break;
                    default:
                    break;
                }
            }
            SST_TimeEvt_arm(&me->te1, 150U, 0U);      /* call periodically to handle IR reception every 150 ms */
            break;
        }
        default: {
            DBC_ERROR(200);
            break;
        }
    }
}


