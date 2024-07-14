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
#ifndef BSP_H_
#define BSP_H_

#define TRUE    0x01
#define FALSE   0x00

// #define SEVEN_SEGMENT
#define BSP_TICKS_PER_SEC 1000U
#define DATA_BUF_SIZE 2048
#define WEBSRV_SN       1
#define SOCKET_SNTP     0

void BSP_init(void);
uint32_t micros(void);


void BSP_ledOn(void);
void BSP_ledOff(void);
void BSP_a0on(void);
void BSP_a0off(void);
void BSP_a1on(void);
void BSP_a1off(void);
void BSP_c0on(void);
void BSP_c0off(void);
void BSP_cs_assert(void);
void BSP_cs_deassert(void);

void BSP_disable_PWM(void);
void BSP_enable_PWM(void);


typedef enum {
    // PULSE_9000=0,
    DELAY_4500=0,
    SEND_ADDR,
    SEND_ADDR_INV,
    SEND_CMD, 
    SEND_CMD_INV,
    PULSE_562, 
    IDLE

}Ir_tx_state; 
#define EXP_COMPENSATION    20U     /* adjustment experimental observation on timing */

void send_ir(uint8_t addr, uint8_t cmd);
void change_ir_tx_state(Ir_tx_state new);

#endif /* BSP_H_ */
