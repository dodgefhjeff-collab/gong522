/**
  ************************************* Copyright ******************************
  *  
  *                         (C) Copyright 2023,,China.
  *                                 All Rights Reserved
  *                              
  *                            By(ܵӿƼ޹˾)
  *                     
  *      
  * ļ       : nvic.h   
  * 汾         : v1.0		
  *          : 			
  * ʱ         : 2023-01-29         
  *          :    
  * б     :  
  	1. ....
  	      <汾>: 		
      <޸Ա>:
  		  <ʱ>:
          <>:  
  	2. ...
  ******************************************************************************
 */
#ifndef __NVIC_H
/* 宏定义说明：__NVIC_H，头文件重复包含保护宏。 */
#define	__NVIC_H
#include "gd32c10x.h"
/* 变量说明：ADC_OK，采样换算结果或ADC通道映射变量。 */
extern uint8_t ADC_OK;

/* 函数声明说明：NVIC_init_all，配置中断优先级和启用所需中断。 */
void NVIC_init_all(void);
/* 函数声明说明：TIM3_IRQHandler，定时器或PWM配置/处理函数。 */
void TIM3_IRQHandler(void);						  //TIM3ж
/* 函数声明说明：DMA1_Channel1_IRQHandler，DMA初始化或中断处理函数。 */
void DMA1_Channel1_IRQHandler(void);		//DMA1ͨ1ж,adc
/* 函数声明说明：DMA1_Channel4_IRQHandler，DMA初始化或中断处理函数。 */
void DMA1_Channel4_IRQHandler(void);		//DMA1ͨ4ж,USART1_TX
/* 函数声明说明：DMA1_Channel5_IRQHandler，DMA初始化或中断处理函数。 */
void DMA1_Channel5_IRQHandler(void);   //DMA1ͨ5ж,USART1_RX
#endif


