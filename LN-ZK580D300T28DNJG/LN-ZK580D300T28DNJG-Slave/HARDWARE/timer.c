/**
  ************************************* Copyright ****************************** 
  *
  *                          (C) Copyright 2023,,China.
  *                                  All Rights Reserved
  *                              
  *                             By(ܵӿƼ޹˾)
  *                     
  *    
  * ļ	   : timer.c   
  * 汾       : v1.0		
  *        : 			
  * ʱ       : 2023-01-29         
  *        :    
  * б   :  
  	1. ....
  	      <汾>: 		
      <޸Ա>:
  		  <ʱ>:
          <>:  
  	2. ...
  ******************************************************************************
 */

#include "timer.h"
#include "gd32c10x.h"
#include <stdio.h>
#include "gpio.h"
#include "power.h"

#if configGENERATE_RUN_TIME_STATS 
//FreeRTOSʱͳõĽļ
/* 变量说明：FreeRTOSRunTimeTicks，FreeRTOSRunTimeTicks变量，用于保存当前模块运行过程中的状态或临时数据。 */
volatile unsigned long long FreeRTOSRunTimeTicks=0;
#endif
/* 变量说明：timer_falg，timer_falg变量，用于保存当前模块运行过程中的状态或临时数据。 */
uint16_t  timer_falg=0;   //10sʱǣ

/**
    \brief      configure the TIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
  */
/*******************************************************************************
** 函数名称: timer1_config
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void timer1_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
    ---------------------------------------------------------------------------- */
    // timer_oc_parameter_struct timer_ocinitpara;
    /* 变量说明：timer_initpara，timer_initpara变量，用于保存当前模块运行过程中的状态或临时数据。 */
    timer_parameter_struct timer_initpara;

    /* ---------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock / 60000 = 2KHz,
    And generate 3 signals with 3 different delays:
    TIMER1_CH0 delay = 2000/2000 = 1s
    TIMER1_CH1 delay = 4000/2000 = 2s
    TIMER1_CH2 delay = 6000/2000 = 3s
    --------------------------------------------------------------- */
    rcu_periph_clock_enable(RCU_AF);

    rcu_periph_clock_enable(RCU_TIMER1);
    timer_deinit(TIMER1);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1, &timer_initpara);
    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER1, TIMER_INT_UP);
    timer_enable(TIMER1);
}

/*******************************************************************************
** 函数名称: timer2_config
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void timer2_config(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
    ---------------------------------------------------------------------------- */
    // timer_oc_parameter_struct timer_ocinitpara;
    /* 变量说明：timer_initpara，timer_initpara变量，用于保存当前模块运行过程中的状态或临时数据。 */
    timer_parameter_struct timer_initpara;

    /* ---------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock / 60000 = 2KHz,
    And generate 3 signals with 3 different delays:
    TIMER1_CH0 delay = 2000/2000 = 1s
    TIMER1_CH1 delay = 4000/2000 = 2s
    TIMER1_CH2 delay = 6000/2000 = 3s
    --------------------------------------------------------------- */
    rcu_periph_clock_enable(RCU_AF);

    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 11999;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 49;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);
    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    timer_enable(TIMER2);
}

#if configGENERATE_RUN_TIME_STATS 
/*******************************************************************************
** 函数名称: ConfigureTimeForRunTimeStats
** 功能描述: ConfigureTimeForRunTimeStats函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void ConfigureTimeForRunTimeStats(void)
{
    /* ----------------------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock/12000 = 10KHz, the period is 1s(10000/10000 = 1s).
    ---------------------------------------------------------------------------- */
    // timer_oc_parameter_struct timer_ocinitpara;
    /* 变量说明：timer_initpara，timer_initpara变量，用于保存当前模块运行过程中的状态或临时数据。 */
    timer_parameter_struct timer_initpara;

    /* ---------------------------------------------------------------
    TIMER1 Configuration:
    TIMER1CLK = SystemCoreClock / 60000 = 2KHz,
    And generate 3 signals with 3 different delays:
    TIMER1_CH0 delay = 2000/2000 = 1s
    TIMER1_CH1 delay = 4000/2000 = 2s
    TIMER1_CH2 delay = 6000/2000 = 3s
    --------------------------------------------------------------- */
    rcu_periph_clock_enable(RCU_AF);

    rcu_periph_clock_enable(RCU_TIMER2);
    timer_deinit(TIMER2);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER1 configuration */
    timer_initpara.prescaler         = 119;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 99;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);
    /* enable the TIMER interrupt */
    timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
    timer_enable(TIMER2);
}
#endif



/**
  * @brief  This function handles TIMER1 interrupt request.
  * @param  None
  * @retval None
  */
/*******************************************************************************
** 函数名称: TIMER1_IRQHandler
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void TIMER1_IRQHandler()
{		
    if(SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP)) {
        /* clear update interrupt bit */
			timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
    }
}


#if configGENERATE_RUN_TIME_STATS 
/*
  * @brief  This function handles TIMER2 interrupt request.
  * @param  None
  * @retval None
  */
/*******************************************************************************
** 函数名称: TIMER2_IRQHandler
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void TIMER2_IRQHandler()
{
    if(SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP)) {
        /* clear update interrupt bit */
			timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
      FreeRTOSRunTimeTicks++;
			
    }
}
#endif




