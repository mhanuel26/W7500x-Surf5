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
#ifndef WEB_SERVER_H_
#define WEB_SERVER_H_

#include "dbc_assert.h" /* Design By Contract (DBC) assertions */
#include "sntp.h"

#define SNTP_REFRESH    3600        /* Refresh Clock Rate in seconds using SNTP*/
#define SNTP_DBG_BLIP   false       /* SNTP uses a Blip signal on DIO C0 of SURF5 */
#define WEB_DBG_BLIP    false       /* SNTP uses a Blip signal on DIO C0 of SURF5 */
/* Timezone */
#define TIMEZONE 21     // Ireland
#define DST_REGION      EU

#define SOCKET_SNTP     2
#define SOCKET_WEB      1

typedef struct {
    SST_Evt super;    /* inherit SST_Evt */

} SntpWorkEvt;

enum WebSignals {
    GET_SN_SR_SIG,
    SNTP_RUN, 
    SNTP_PAUSE, 
    /* ... */
    MAX_SERVER_SIG  /* the last signal */
};


/* web server prototypes */
void Webserver_instantiate(void);
bool Webserver_get_phyready(void);
void Webserver_set_phyready(void);
/* sntp server prototypes */
void Sntpserver_instantiate(void);
extern SST_Task * const AO_Server;  /* opaque task pointer */
extern SST_Task * const AO_Sntp;  /* opaque task pointer */

#endif /* WEB_SERVER_H_ */
