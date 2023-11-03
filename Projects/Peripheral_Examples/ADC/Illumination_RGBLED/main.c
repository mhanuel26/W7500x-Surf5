/*******************************************************************************************************************************************************
 * Copyright �� 2016 <WIZnet Co.,Ltd.> 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the ��Software��), 
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED ��AS IS��, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*********************************************************************************************************************************************************/
/**
  ******************************************************************************
  * @file    ADC/Illumination_RGBLED/main.c
  * @author  IOP Team
  * @version V1.0.0
  * @date    25-AUG-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WIZnet SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2015 WIZnet Co.,Ltd.</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "W7500x.h"
#include "W7500x_adc.h"
#include "W7500x_uart.h"
#include "W7500x_gpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
int sensorValue_CH2;
	
/* Private function prototypes -----------------------------------------------*/
void UART_Configuration(void);
void delay_ms(__IO uint32_t nCount);
int ADC_Read(ADC_CH num);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */


int main()
{
    /*System clock configuration*/
    SystemInit();
    /* UART configuration */
    UART_Configuration();
    printf("***********         WIZnet         ***********\r\n");

    // ADC initialize
    ADC_Init();
    while(1) {
    sensorValue_CH2 = ADC_Read(ADC_CH2);
	printf("Sensor value CH2= %d\r\n",sensorValue_CH2);
	delay_ms(100);
    }
}

void delay_ms(__IO uint32_t nCount)
{
    volatile uint32_t delay = nCount * 2500; // approximate loops per ms at 24 MHz, Debug config
    for(; delay != 0; delay--)
        __NOP();
}

int ADC_Read(ADC_CH num)
{
    ADC_ChannelSelect (num); ///< Select ADC channel to CH2
    ADC_Start(); ///< Start ADC
    while(ADC_IsEOC()); ///< Wait until End of Conversion
    return ((uint16_t)ADC_ReadData()); ///< read ADC Data
}

void UART_Configuration(void)
{
     UART_InitTypeDef UART_InitStructure;

     /* UART Configuration for UART1 & UART2*/
     UART_StructInit(&UART_InitStructure);
     UART_Init(UART1,&UART_InitStructure);
     S_UART_Init(115200);
}
