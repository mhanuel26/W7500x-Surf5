/*============================================================================
* Super-Simple Tasker (SST/C) Example for WIZNET W7500P SURF5 BOARD. 
*
*                    Q u a n t u m  L e a P s
*                    ------------------------
*                    Modern Embedded Software
*
* Copyright (C) 2024 Quantum Leaps, <state-machine.com>.
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
#include "sst.h"
#include "bsp.h"
#include "blinky.h"
#include "server.h"
#include "led_matrix.h"
#include "max7219.h"
#include <string.h>
#include "w7500x.h"  /* CMSIS-compliant header file for the MCU used */
/* add other drivers if necessary... */
#include "wizchip_conf.h"
#include "ir_ctrl_task.h"
#include "dhcp.h"
#include "sntp.h"
DBC_MODULE_NAME("bsp_surf5_w7500") /* for DBC assertions in this module */

uint8_t srv_buf[DATA_BUF_SIZE];
wiz_NetInfo gWIZNETINFO;
char cTemp[32];
uint8_t bImg[BYTES_PER_LINE*8];
/* SNTP */
static uint8_t g_sntp_buf[DATA_BUF_SIZE] = {
    0,
};
static uint8_t g_sntp_server_ip[4] = {216, 239, 35, 0}; // time.google.com

static uint32_t micros_last;
static uint32_t micros_acc = 0;
/* Local-scope defines -----------------------------------------------------*/
static void BSP_GPIO_Config(void);
static void BSP_UART_Config(void);
static void DUALTIMER_Config(void);
static void Network_Config(void);
void dhcp_assign(void);
void dhcp_update(void);
void dhcp_conflict(void);


/* ISRs used in the application ============================================*/
void SysTick_Handler(void);  /* prototype */

void SysTick_Handler(void) { /* system clock tick ISR */
    SST_TimeEvt_tick(); /* process all SST time events */
}

/* Assertion handler ======================================================*/
DBC_NORETURN
void DBC_fault_handler(char const * const module, int const label) {
    /*
    * NOTE: add here your application-specific error handling
    */
    (void)module;
    (void)label;

    /* set PRIMASK to disable interrupts and stop SST right here */
    __asm volatile ("cpsid i");

#ifndef NDEBUG
    for (;;) { /* keep blinking LED2 */
        BSP_ledOn();  /* turn LED2 on */
        uint32_t volatile ctr;
        for (ctr = 10000U; ctr > 0U; --ctr) {
        }
        BSP_ledOff(); /* turn LED2 off */
        for (ctr = 10000U; ctr > 0U; --ctr) {
        }
    }
#endif
    NVIC_SystemReset();
}
/*..........................................................................*/
void assert_failed(char const * const module, int const label);/* prototype */
void assert_failed(char const * const module, int const label) {
    DBC_fault_handler(module, label);
}

/* SST task activations ====================================================*/
/* preprocessor switch to choose between regular and reserved IRQs */
#define REGULAR_IRQS

#ifdef REGULAR_IRQS
/* repurpose regular IRQs for SST Tasks */

/**
 * @brief  This function handles PWM4 Handler for SSR Task IR Remote Control.
 * @param  None
 * @retval None
 */
void PWM4_Handler(void)
{
    SST_Task_activate(AO_IrCtrl);  
}
/* prototypes */
/**
 * @brief  This function handles RTC Handler for SST Task WebServer.
 * @param  None
 * @retval None
 */
void RTC_Handler(void)
{
    SST_Task_activate(AO_Server);  
}
/**
 * @brief  This function handles PWM7 for Blinky Task
 * @param  None
 * @retval None
 */
void PWM7_Handler(void);

void PWM7_Handler(void)	{ 
    SST_Task_activate(AO_Blinky);  
}
/**
 * @brief  This function handles PWM6 for Led Matrix Task
 * @param  None
 * @retval None
 */
void PWM6_Handler(void);

void PWM6_Handler(void)	{ 
    SST_Task_activate(AO_Matrix);  
} 
/**
 * @brief  This function handles PWM7 for Blinky Task
 * @param  None
 * @retval None
 */
void PWM5_Handler(void);

void PWM5_Handler(void)	{ 
    SST_Task_activate(AO_Sntp);  
} 

#else /* use reserved IRQs for SST Tasks */
/* prototypes */
void Reserved14_IRQHandler(void);

void Reserved14_IRQHandler(void) { SST_Task_activate(AO_Blinky);  }
#endif

/* BSP functions ===========================================================*/
void BSP_init(void) {
    /* Configure the MPU is not possible on ARM-Cortex M0 */

    __ISB();
    __DSB();

    /* assign IRQs to tasks. NOTE: critical for SST... */
#ifdef REGULAR_IRQS
    /* repurpose regular IRQs for SST Tasks */
    SST_Task_setIRQ(AO_IrCtrl,  PWM4_IRQn);
    SST_Task_setIRQ(AO_Sntp,    PWM5_IRQn);
    SST_Task_setIRQ(AO_Matrix,  PWM6_IRQn);
    SST_Task_setIRQ(AO_Blinky,  PWM7_IRQn);
    SST_Task_setIRQ(AO_Server,  RTC_IRQn);
#else
    /* use reserved IRQs for SST Tasks */
    SST_Task_setIRQ(AO_Blinky,  14U);
#endif

    /* Initialize the GPIO */
    BSP_GPIO_Config();
}


/**
 * @brief  Configures the UART Peripheral.
 * @note
 * @param  None
 * @retval None
 */
static void BSP_UART_Config(void)
{
    UART_InitTypeDef UART_InitStructure;

    UART_StructInit(&UART_InitStructure);

#if defined (USE_WIZWIKI_W7500_EVAL)
    UART_Init(UART1, &UART_InitStructure);
    UART_Cmd(UART1, ENABLE);
#else
    S_UART_Init(115200);
    S_UART_Cmd(ENABLE);
#endif
}



/*..........................................................................*/
/**
 * @brief  Configures the GPIO Peripheral.
 * @note
 * @param  None
 * @retval None
 */
static void BSP_GPIO_Config(void)
{
    /* Surf5 GPIO init routine*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_ITInitTypeDef GPIO_ITInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Init LED GPIO of Surf5 board */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_OUT;
    GPIO_InitStructure.GPIO_AF = PAD_AF1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    /* Init a GPIO for Web Task Debug Monitoring using Logic Analyser*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_OUT;
    GPIO_InitStructure.GPIO_AF = PAD_AF0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Init a GPIO for LED MATRIX Debug Monitoring using Logic Analyser*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_OUT;
    GPIO_InitStructure.GPIO_AF = PAD_AF0;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /* Init a GPIO for SNTP Task Debug Monitoring using Logic Analyser*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_OUT;
    GPIO_InitStructure.GPIO_AF = PAD_AF1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);    

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Direction = GPIO_Direction_IN;
    GPIO_InitStructure.GPIO_Pad = GPIO_InputBufferEnable | GPIO_CMOS | GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_AF = PAD_AF1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_ITInitStructure.GPIO_IT_Pin = GPIO_Pin_4;
    GPIO_ITInitStructure.GPIO_IT_Polarity = GPIO_IT_LowFalling; //GPIO_IT_HighRising;
    GPIO_ITInitStructure.GPIO_IT_Type = GPIO_IT_Edge;
    GPIO_IT_Init(GPIOC, &GPIO_ITInitStructure);
    GPIO_ITConfig(GPIOC, GPIO_Pin_4, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = PORT2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/* SURF5 LED2 */
void BSP_ledOn(void)  { GPIO_ResetBits(GPIOC, GPIO_Pin_15);       } 
void BSP_ledOff(void) { GPIO_SetBits(GPIOC, GPIO_Pin_15); }
/* SURF5 GPIOA 0 */
void BSP_a0on(void) { GPIO_SetBits(GPIOA, GPIO_Pin_0); }
void BSP_a0off(void) { GPIO_ResetBits(GPIOA, GPIO_Pin_0); }
/* SURF5 GPIOA 1 */
void BSP_a1on(void) { GPIO_SetBits(GPIOA, GPIO_Pin_1); }
void BSP_a1off(void) { GPIO_ResetBits(GPIOA, GPIO_Pin_1); }
/* SURF5 GPIOC 0 */
void BSP_c0on(void) { GPIO_SetBits(GPIOC, GPIO_Pin_0); }
void BSP_c0off(void) { GPIO_ResetBits(GPIOC, GPIO_Pin_0); }

void BSP_cs_assert(void) { GPIO_ResetBits(GPIOA, GPIO_Pin_5); }
void BSP_cs_deassert(void) { GPIO_SetBits(GPIOA, GPIO_Pin_5); }


/**
 * @brief  Configures the Network Information.
 * @note
 * @param  None
 * @retval None
 */
static void Network_Config(void)
{
    uint8_t mac_addr[6] = { 0x00, 0x08, 0xDC, 0x01, 0x02, 0x03 };

    memcpy(gWIZNETINFO.mac, mac_addr, 6);
    gWIZNETINFO.dhcp = NETINFO_DHCP;

    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);

    printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n", gWIZNETINFO.mac[0], gWIZNETINFO.mac[1], gWIZNETINFO.mac[2], gWIZNETINFO.mac[3], gWIZNETINFO.mac[4], gWIZNETINFO.mac[5]);
    printf("IP: %d.%d.%d.%d\r\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
    printf("GW: %d.%d.%d.%d\r\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
    printf("SN: %d.%d.%d.%d\r\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
    printf("DNS: %d.%d.%d.%d\r\n", gWIZNETINFO.dns[0], gWIZNETINFO.dns[1], gWIZNETINFO.dns[2], gWIZNETINFO.dns[3]);
}

/**
 * @brief  The call back function of ip assign.
 * @note
 * @param  None
 * @retval None
 */
void dhcp_assign(void)
{
    getIPfromDHCP(gWIZNETINFO.ip);
    getGWfromDHCP(gWIZNETINFO.gw);
    getSNfromDHCP(gWIZNETINFO.sn);
    getDNSfromDHCP(gWIZNETINFO.dns);

    ctlnetwork(CN_SET_NETINFO, (void*) &gWIZNETINFO);
}

/**
 * @brief  The call back function of ip update.
 * @note
 * @param  None
 * @retval None
 */
void dhcp_update(void)
{
    ;
}

/**
 * @brief  The call back function of ip conflict.
 * @note
 * @param  None
 * @retval None
 */
void dhcp_conflict(void)
{
    ;
}

/**
 * @brief  Configures the DUALTIMER Peripheral.
 * @note
 * @param  None
 * @retval None
 */
static void DUALTIMER_Config(void)
{
    DUALTIMER_InitTypDef DUALTIMER_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    DUALTIMER_InitStructure.Timer_Load = GetSystemClock() / 1; //1s
    DUALTIMER_InitStructure.Timer_Prescaler = DUALTIMER_Prescaler_1;
    DUALTIMER_InitStructure.Timer_Wrapping = DUALTIMER_Periodic;
    DUALTIMER_InitStructure.Timer_Repetition = DUALTIMER_Wrapping;
    DUALTIMER_InitStructure.Timer_Size = DUALTIMER_Size_32;
    DUALTIMER_Init(DUALTIMER0_0, &DUALTIMER_InitStructure);

    DUALTIMER_ITConfig(DUALTIMER0_0, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = DUALTIMER0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    DUALTIMER_Cmd(DUALTIMER0_0, ENABLE);

    /* Free Running Timer for IR Remote micros() function */
    DUALTIMER_InitStructure.Timer_Load = GetSystemClock() / 1;
    DUALTIMER_InitStructure.Timer_Prescaler = DUALTIMER_Prescaler_16;
    DUALTIMER_InitStructure.Timer_Wrapping = DUALTIMER_Free_Running; //DUALTIMER_Periodic;
    DUALTIMER_InitStructure.Timer_Repetition = DUALTIMER_Wrapping;
    DUALTIMER_InitStructure.Timer_Size = DUALTIMER_Size_32;
    DUALTIMER_Init(DUALTIMER1_0, &DUALTIMER_InitStructure);


    DUALTIMER_Cmd(DUALTIMER1_0, ENABLE);

    micros_last = DUALTIMER_GetValue(DUALTIMER1_0);

}

uint32_t micros(void){
    uint32_t micros;
    uint32_t tmp =  DUALTIMER_GetValue(DUALTIMER1_0);
    if(tmp > micros_last){
        uint32_t roll_over_count = 0xFFFFFFFF - tmp;
        micros = micros_last + roll_over_count;
    }else{
        micros = micros_last - tmp;
    }
    micros_last = tmp;
    micros_acc += micros;
    return micros_acc;
}


/* SST callbacks ===========================================================*/
void SST_onStart(void) {
    uint32_t ret;
    uint8_t dhcp_retry = 0;
    int rc;
    int iNumControllers, iSegmentMode;
#ifdef SEVEN_SEGMENT
	iNumControllers = 1;
	iSegmentMode = 1;
#else
	iNumControllers = 4; // assume 4 x 8x8 array
	iSegmentMode = 0;
#endif
    NVIC_InitTypeDef NVIC_InitStructure;
    SystemInit();
    // SystemCoreClockUpdate();

    /* set up the SysTick timer to fire at BSP_TICKS_PER_SEC rate */
    SysTick_Config((GetSystemClock() / BSP_TICKS_PER_SEC) + 1U);

    /* Set WZ_100US Register */
    setTIC100US((GetSystemClock() / 10000));

    /* setup uart for debug console output using printf*/
    BSP_UART_Config();
    DUALTIMER_Config();

    printf("W7500x Standard Peripheral Library version : %d.%d.%d\r\n", __W7500X_STDPERIPH_VERSION_MAIN, __W7500X_STDPERIPH_VERSION_SUB1, __W7500X_STDPERIPH_VERSION_SUB2);

    printf("SourceClock : %d\r\n", (int) GetSourceClock());
    printf("SystemClock : %d\r\n", (int) GetSystemClock());

    /* set priorities of ISRs used in the system */

    /* PWM4 ISR for IR Remote Control Task */
    NVIC_InitStructure.NVIC_IRQChannel = PWM4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* PWM5 ISR for Blinky */
    NVIC_InitStructure.NVIC_IRQChannel = PWM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* PWM7 ISR for Blinky */
    NVIC_InitStructure.NVIC_IRQChannel = PWM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* PWM6 ISR for LED Matrix Task */
    NVIC_InitStructure.NVIC_IRQChannel = PWM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    /* RTC ISR for Web Server Task */
    NVIC_InitStructure.NVIC_IRQChannel = SysTick_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPriority = 0x0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_SetPriority(SysTick_IRQn, 0U);
    /* ... */

    /* Initialize PHY */
#ifdef W7500
    printf("PHY Init : %s\r\n", PHY_Init(GPIOB, GPIO_Pin_15, GPIO_Pin_14) == SET ? "Success" : "Fail");
#elif defined (W7500P)
    printf("PHY Init : %s\r\n", PHY_Init(GPIOB, GPIO_Pin_14, GPIO_Pin_15) == SET ? "Success" : "Fail");
#endif

    /* Check Link */
    printf("Link : %s\r\n", PHY_GetLinkStatus() == PHY_LINK_ON ? "On" : "Off");

    /* Network information setting before DHCP operation. Set only MAC. */
    Network_Config();

    /* DHCP Process */
    DHCP_init(0, srv_buf);
    reg_dhcp_cbfunc(dhcp_assign, dhcp_update, dhcp_conflict);
    if (gWIZNETINFO.dhcp == NETINFO_DHCP) {       // DHCP
        printf("Start DHCP\r\n");
        while (1) {
            ret = DHCP_run();

            if (ret == DHCP_IP_LEASED) {
                printf("DHCP Success\r\n");
                break;
            }
            else if (ret == DHCP_FAILED) {
                dhcp_retry++;
            }

            if (dhcp_retry > 3) {
                printf("DHCP Fail\r\n");
                break;
            }
        }
    }

    /* Network information setting after DHCP operation.    
     * Displays the network information allocated by DHCP. */
    Network_Config();
    /* Init SNTP Stack */
    SNTP_init(SOCKET_SNTP, g_sntp_server_ip, TIMEZONE, DST_REGION, g_sntp_buf);
    /* signal our webserver task the PHY is ready */
    // Webserver_set_phyready();

    /* Send event to SNTP task */
    static SntpWorkEvt const PhyInitDoneEvt = {
        .super.sig = SNTP_RUN,
        // .super.sig = GET_SN_SR_SIG,
        
    };
    SST_Task_post(AO_Sntp, &PhyInitDoneEvt.super);

    /* Initialize the dot-matrix library
	 * num controllers, BCD mode, SPI channel, GPIO pin number for CS*/ 
	rc = maxInit(iNumControllers, iSegmentMode, 0, 0);
	if (rc != 0)
	{
		printf("Problem initializing max7219\n");
	}
    maxSetIntensity(4);    

#ifdef SEVEN_SEGMENT
    /* Send event to 7 segment max7219 controller */
    static MatrixWorkEvt const fInitDoneEvt = {
        .super.sig = USER_ONE_SHOT,
        .text = "00000000"
    };
    SST_Task_post(AO_Matrix, &fInitDoneEvt.super);
#else
    /* Send event to led matrix max7219 controller */
    static MatrixWorkEvt const fInitDoneEvt = {
        .super.sig = USER_ONE_SHOT,
        .text = "Surf5 Demo using SST RTOS.",
        .scroll_iter = 1        // 0 here for one shot image send or a number for times text will scroll
    };
    SST_Task_post(AO_Matrix, &fInitDoneEvt.super);
#endif
}
/*..........................................................................*/
void SST_onIdle(void) {
#ifdef NDEBUG
    /* Put the CPU and peripherals to the low-power mode.
    * you might need to customize the clock management for your application,
    * see the datasheet for your particular Cortex-M MCU.
    */
    __WFI(); /* Wait-For-Interrupt */
#endif
}

