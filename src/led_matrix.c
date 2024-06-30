/*============================================================================
* Super-Simple Tasker (SST/C) Example
*
* Copyright (C) 2006-2023 Quantum Leaps, <state-machine.com>.
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
#include "led_matrix.h"        /* application shared interface */
#include <stdio.h>
#include "max7219.h"

DBC_MODULE_NAME("led_matrix")  /* for DBC assertions in this module */

/* Matrix event-driven task ------------------------------------------------*/
typedef struct {
    SST_Task super; /* inherit SST_Task */

    SST_TimeEvt te1;
    SST_TimeEvt te2;
    SST_TimeEvt te3;
} Matrix;

static void Matrix_ctor(Matrix * const me);
static void Matrix_init(Matrix * const me, SST_Evt const * const ie);
static void Matrix_dispatch(Matrix * const me, SST_Evt const * const e);

/*..........................................................................*/
static Matrix Matrix_inst; /* the Matrix instance */
SST_Task * const AO_Matrix = &Matrix_inst.super; /* opaque AO pointer */

/*..........................................................................*/
void Matrix_instantiate(void) {
    Matrix_ctor(&Matrix_inst);
}
/*..........................................................................*/
void Matrix_ctor(Matrix * const me) {
    SST_Task_ctor(
       &me->super,
       (SST_Handler)&Matrix_init,
       (SST_Handler)&Matrix_dispatch);
    SST_TimeEvt_ctor(&me->te1, SCROLL_MATRIX, &me->super);
    SST_TimeEvt_ctor(&me->te2, USER_SIG1, &me->super);
    SST_TimeEvt_ctor(&me->te3, USER_ONE_SHOT, &me->super);
}


/*--------------------------------------------------------------------------*/
static void Matrix_init(Matrix * const me, SST_Evt const * const ie) {
    (void)ie; /* unused parameter */

    // SST_TimeEvt_arm(&me->te3, 1U, 0U);
}
/*..........................................................................*/
static void Matrix_dispatch(Matrix * const me, SST_Evt const * const e) {
    switch (e->sig) {
        case SCROLL_MATRIX: {
            BSP_a1on();
            SST_TimeEvt_arm(&me->te2, BSP_TICKS_PER_SEC / 4U, 0U);
            BSP_a1off();
            break;
        }
        case USER_SIG1: {
            BSP_a1on();
            BSP_a1off();
            SST_TimeEvt_arm(&me->te1, BSP_TICKS_PER_SEC * 3U/4U, 0U);
            break;
        }
        case USER_ONE_SHOT: {
            BSP_a1on();
            const char * tmp = SST_EVT_DOWNCAST(MatrixWorkEvt, e)->text;
            maxSegmentString((char*)tmp);
            BSP_a1off();
            break;
        }
        default: {
            DBC_ERROR(200);
            break;
        }
    }
}