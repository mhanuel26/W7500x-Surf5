/*============================================================================
* Super-Simple Tasker (SST/C) Example for LED MATRIX OR 7 SEGMENT USING MAX7219
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
#ifndef LED_MATRIX_H_
#define LED_MATRIX_H_

#include "dbc_assert.h" /* Design By Contract (DBC) assertions */

#define BYTES_PER_LINE  24

#define FONT_SIZE_6X8       

typedef struct {
    SST_Evt super;    /* inherit SST_Evt */
    char text[40]; /* number of toggles of the signal */
    uint32_t scroll_iter;
} MatrixWorkEvt;

enum MatrixSignals {
    SCROLL_MATRIX,
    USER_SIG1, 
    USER_ONE_SHOT, 
    /* ... */
    LED_MATRIX_MAX_SIG  /* the last signal */
};

void Matrix_instantiate(void);
extern SST_Task * const AO_Matrix;  /* opaque task pointer */

#endif /* BLINKY_LED_MATRIX_H_H_ */
