/**
 ******************************************************************************
 * @file    w7500x_it.c
 * @author  WIZnet
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2018 WIZnet</center></h2>
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "w7500x_it.h"

#include "nec_decoder.h"
#include "dhcp.h"
#include "bsp.h"

uint32_t tmp_dif = 0;
static Ir_tx_state state = IDLE;
static uint8_t ir_addr, ir_cmd, ir_addr_inv, ir_cmd_inv;
static uint8_t ir_cnt = 0;
static bool ir_pulse_dly = true;
/** @addtogroup W7500x_StdPeriph_Examples
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void send_ir(uint8_t addr, uint8_t cmd){
    if(state == IDLE){
        ir_addr = addr;
        ir_addr_inv = ~addr;
        ir_cmd = cmd;
        ir_cmd_inv = ~cmd;
        ir_pulse_dly = true;
        change_ir_tx_state(DELAY_4500);
        PWM_Cmd(PWM2, ENABLE);
        DUALTIMER_SetLoad(DUALTIMER1_0, 9000U);
        DUALTIMER_Cmd(DUALTIMER1_0, ENABLE);
    }
}

void change_ir_tx_state(Ir_tx_state new){
    state = new;
}


void send_ir_data(uint8_t* data, Ir_tx_state next_state){
    if(ir_pulse_dly){
        PWM_Cmd(PWM2, ENABLE);  /* activate the pulse */
        DUALTIMER_SetLoad(DUALTIMER1_0, 562U - EXP_COMPENSATION);
    }else{
        PWM_Cmd(PWM2, DISABLE);
        if (*data & (1 << 7)) DUALTIMER_SetLoad(DUALTIMER1_0, 1687U - EXP_COMPENSATION);
        else DUALTIMER_SetLoad(DUALTIMER1_0, 562U);
        *data <<= 1;
        if(ir_cnt >= 7){
            state = next_state;
            ir_cnt = 0;
        }else{
            ir_cnt++;        /* here we ack one less bit to send */
        }
    }
    ir_pulse_dly ^= true;
}


/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles PendSV_Handler exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
//void SysTick_Handler(void)
//{
//}

/******************************************************************************/
/*                 W7500x Peripherals Interrupt Handlers                      */
/*    Add here the Interrupt Handler for the used peripherals                 */
/******************************************************************************/

/**
 * @brief  This function handles SSP0 Handler.
 * @param  None
 * @retval None
 */
void SSP0_Handler(void)
{
}

/**
 * @brief  This function handles SSP1 Handler.
 * @param  None
 * @retval None
 */
void SSP1_Handler(void)
{
}

/**
 * @brief  This function handles UART0 Handler.
 * @param  None
 * @retval None
 */
void UART0_Handler(void)
{
}

/**
 * @brief  This function handles UART1 Handler.
 * @param  None
 * @retval None
 */
void UART1_Handler(void)
{
}

/**
 * @brief  This function handles UART2 Handler.
 * @param  None
 * @retval None
 */
void UART2_Handler(void)
{
}

/**
 * @brief  This function handles PORT0 Handler.
 * @param  None
 * @retval None
 */
void PORT0_Handler(void)
{
}

/**
 * @brief  This function handles PORT1 Handler.
 * @param  None
 * @retval None 
 */
void PORT1_Handler(void)
{
}
/*
        // uint32_t tmp = micros();
        // uint32_t tmp2 =  tmp - tmp_dif;
        // tmp_dif = tmp;
        // printf("micros diff = %lu\r\n", tmp2);
*/
/**
 * @brief  This function handles PORT2 Handler.
 * @param  None
 * @retval None
 */
void PORT2_Handler(void)
{
    if (GPIO_GetITStatus(GPIOC, GPIO_Pin_4) == SET) {
        nec_decoder_tick();
        GPIO_ClearITPendingPin(GPIOC, GPIO_Pin_4);
    }
}

/**
 * @brief  This function handles PORT3 Handler.
 * @param  None
 * @retval None
 */
void PORT3_Handler(void)
{
}

/**
 * @brief  This function handles DMA Handler.
 * @param  None
 * @retval None
 */
void DMA_Handler(void)
{
}

/**
 * @brief  This function handles DUALTIMER0 Handler.
 * @param  None
 * @retval None
 */
void DUALTIMER0_Handler(void)
{
    if (DUALTIMER_GetITStatus(DUALTIMER0_0)) {
        DUALTIMER_ClearIT(DUALTIMER0_0);
        DHCP_time_handler();
    }
}


/**
 * @brief  This function handles DUALTIMER1 Handler.
 * @param  None
 * @retval None
 */
void DUALTIMER1_Handler(void)
{
    if (DUALTIMER_GetITStatus(DUALTIMER1_0)) {
        DUALTIMER_ClearIT(DUALTIMER1_0);
        switch(state){
            case IDLE:
                DUALTIMER_Cmd(DUALTIMER1_0, DISABLE);
                PWM_Cmd(PWM2, DISABLE);
            break;
            case DELAY_4500:
                PWM_Cmd(PWM2, DISABLE);
                DUALTIMER_SetLoad(DUALTIMER1_0, 4500U);
                state = SEND_ADDR;
                ir_cnt = 0;
            break;
            case SEND_ADDR:
                send_ir_data(&ir_addr, SEND_ADDR_INV);
            break;
            case SEND_ADDR_INV:
                send_ir_data(&ir_addr_inv, SEND_CMD);
            break;
            case SEND_CMD:
                send_ir_data(&ir_cmd, SEND_CMD_INV);
            break;
            case SEND_CMD_INV:
                send_ir_data(&ir_cmd_inv, PULSE_562);
            break;
            case PULSE_562:
                PWM_Cmd(PWM2, ENABLE);
                DUALTIMER_SetLoad(DUALTIMER1_0, 562U);
                state = IDLE;
            break;
            default:
            break;
        }
    }
}

/**
 * @brief  This function handles PWM0 Handler.
 * @param  None
 * @retval None
 */
void PWM0_Handler(void)
{
}

/**
 * @brief  This function handles PWM1 Handler.
 * @param  None
 * @retval None
 */
void PWM1_Handler(void)
{
}

/**
 * @brief  This function handles PWM2 Handler.
 * @param  None
 * @retval None
 */
void PWM2_Handler(void)
{
}

/**
 * @brief  This function handles PWM3 Handler.
 * @param  None
 * @retval None
 */
void PWM3_Handler(void)
{
}

/**
 * @brief  This function handles PWM4 Handler.
 * @param  None
 * @retval None
 */
// void PWM4_Handler(void)
// {
// }

/**
 * @brief  This function handles PWM5 Handler.
 * @param  None
 * @retval None
 */
// void PWM5_Handler(void)
// {
// }

/**
 * @brief  This function handles PWM6 Handler.
 * @param  None
 * @retval None
 */
// void PWM6_Handler(void)
// {
// }

/**
 * @brief  This function handles PWM7 Handler.
 * @param  None
 * @retval None
 */
//void PWM7_Handler(void)
//{
//}

/**
 * @brief  This function handles RTC Handler.
 * @param  None
 * @retval None
 */
// void RTC_Handler(void)
// {
// }

/**
 * @brief  This function handles ADC Handler.
 * @param  None
 * @retval None
 */
void ADC_Handler(void)
{
}

/**
 * @brief  This function handles WATOE Handler.
 * @param  None
 * @retval None
 */
void WZTOE_Handler(void)
{
}

/**
 * @brief  This function handles EXTI Handler.
 * @param  None
 * @retval None
 */
void EXTI_Handler(void)
{
}

/**
 * @}
 */

/******************** (C) COPYRIGHT WIZnet *****END OF FILE********************/
