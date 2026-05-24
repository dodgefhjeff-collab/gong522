/**
  ************************************* Copyright ******************************
  *  
  *                         (C) Copyright 2023,,China.
  *                                 All Rights Reserved
  *                              
  *                            By(ܵӿƼ޹˾)
  *                     
  *      
  * ļ       : usart.h   
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
#ifndef __USART_H
/* 宏定义说明：__USART_H，头文件重复包含保护宏。 */
#define __USART_H
#include "gd32c10x.h"
#include <stdio.h>
/* 宏定义说明：USART_REC_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART_REC_LEN  	    20  	//ֽ 
/* 宏定义说明：USART_TX_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART_TX_LEN        45   //ֽ 

 /* 宏定义说明：USART1_REC_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
 #define USART1_REC_LEN  	20  	//ֽ 
 /* 宏定义说明：USART1_TX_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
 #define USART1_TX_LEN     25   //ֽ 

// #define USART2_REC_LEN  	10  	//ֽ 
// #define USART2_TX_LEN        1   //ֽ 

/* 宏定义说明：USART_REC_IDLE_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART_REC_IDLE_LEN    100
/* 宏定义说明：USART1_REC_IDLE_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART1_REC_IDLE_LEN   100
/* 宏定义说明：USART2_REC_IDLE_LEN，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART2_REC_IDLE_LEN   1
// ڹ궨
/* 宏定义说明：DEBUG_USARTx，调试串口相关宏。 */
#define  DEBUG_USARTx                   USART0
/* 宏定义说明：DEBUG_USART_CLK，调试串口相关宏。 */
#define  DEBUG_USART_CLK                RCC_APB2Periph_USART1
/* 宏定义说明：DEBUG_USART_APBxClkCmd，调试串口相关宏。 */
#define  DEBUG_USART_APBxClkCmd         RCC_APB2PeriphClockCmd
/* 宏定义说明：DEBUG_USART_BAUDRATE，调试串口相关宏。 */
#define  DEBUG_USART_BAUDRATE           115200

// USART GPIO ź궨
/* 宏定义说明：DEBUG_USART_GPIO_CLK，调试串口相关宏。 */
#define  DEBUG_USART_GPIO_CLK           (RCC_APB2Periph_GPIOA)
/* 宏定义说明：DEBUG_USART_GPIO_APBxClkCmd，调试串口相关宏。 */
#define  DEBUG_USART_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd
    
/* 宏定义说明：DEBUG_USART_TX_GPIO_PORT，调试串口相关宏。 */
#define  DEBUG_USART_TX_GPIO_PORT       GPIOA   
/* 宏定义说明：DEBUG_USART_TX_GPIO_PIN，调试串口相关宏。 */
#define  DEBUG_USART_TX_GPIO_PIN        GPIO_Pin_9
/* 宏定义说明：DEBUG_USART_RX_GPIO_PORT，调试串口相关宏。 */
#define  DEBUG_USART_RX_GPIO_PORT       GPIOA
/* 宏定义说明：DEBUG_USART_RX_GPIO_PIN，调试串口相关宏。 */
#define  DEBUG_USART_RX_GPIO_PIN        GPIO_Pin_10

/* 宏定义说明：DEBUG_USART_IRQ，调试串口相关宏。 */
#define  DEBUG_USART_IRQ                USART1_IRQn
/* 宏定义说明：DEBUG_USART_IRQHandler，调试串口相关宏。 */
#define  DEBUG_USART_IRQHandler         USART1_IRQHandler
/* 宏定义说明：USART_DR_ADDRESS，串口/协议缓存、外设或SLIP处理相关宏。 */
#define  USART_DR_ADDRESS        (USART1_BASE+0x04)

/* 变量说明：USART_RX_BUF，串口接收缓冲，保留原工程接口。 */
extern uint8_t  USART_RX_BUF[USART_REC_IDLE_LEN]; //ջ,USART_REC_LENֽ.ĩֽΪз 
/* 变量说明：USART_TX_BUF，串口发送缓冲，保留原工程接口。 */
extern uint8_t  USART_TX_BUF[USART_TX_LEN];

/* 变量说明：USART1_RX_BUF，USART1接收缓冲，28PIN/48PIN内部PA3接收使用。 */
extern uint8_t  USART1_RX_BUF[USART1_REC_IDLE_LEN]; //ջ,USART_REC_LENֽ.ĩֽΪз 
/* 变量说明：USART1_TX_BUF，USART1发送缓冲，28PIN/48PIN内部PA2发送使用。 */
extern uint8_t  USART1_TX_BUF[USART1_TX_LEN];

// extern uint8_t  USART2_RX_BUF[USART2_REC_IDLE_LEN]; //ջ,USART_REC_LENֽ.ĩֽΪз 
// extern uint8_t  USART2_TX_BUF[USART2_TX_LEN];
/* 变量说明：USART_RX_STA，串口接收、发送或状态缓存变量。 */
extern uint8_t USART_RX_STA;         		//״̬	
/* 变量说明：USART1_RX_STA，串口接收、发送或状态缓存变量。 */
extern uint8_t USART1_RX_STA;
// extern uint8_t USART2_RX_STA;


/* 宏定义说明：RS422_1_RX_BUF，RS422_1_RX_BUF宏定义，用于保持原工程风格并集中配置相关参数。 */
#define RS422_1_RX_BUF USART1_RX_BUF
/* 宏定义说明：RS422_2_RX_BUF，RS422_2_RX_BUF宏定义，用于保持原工程风格并集中配置相关参数。 */
#define RS422_2_RX_BUF USART2_RX_BUF


//
/* 函数声明说明：usart_Sen_dma，usart_Sen_dma函数，保持原工程接口并完成对应模块处理。 */
void usart_Sen_dma(uint8_t uartx, uint8_t *buff, uint16_t len);         //DMAһ  
/* 函数声明说明：uart_init，串口初始化、收发或中断处理函数。 */
void uart_init(uint32_t bound);																						//ʼ
/* 函数声明说明：uart1_init，串口初始化、收发或中断处理函数。 */
void uart1_init(uint32_t bound);	
/* 函数声明说明：uart2_init，串口初始化、收发或中断处理函数。 */
void uart2_init(uint32_t bound);
/* 函数声明说明：uart1_rx_byte_process，串口初始化、收发或中断处理函数。 */
void uart1_rx_byte_process(uint8_t ch);
/* 函数声明说明：uart1_get_rx_byte，串口初始化、收发或中断处理函数。 */
uint8_t uart1_get_rx_byte(uint8_t *ch);
/* 函数声明说明：uart1_clear_rx_buff，串口初始化、收发或中断处理函数。 */
void uart1_clear_rx_buff(void);
/* 函数声明说明：Usart_SendByte，串口初始化、收发或中断处理函数。 */
void Usart_SendByte( uint32_t, uint8_t);												//һֽ 
/* 函数声明说明：Usart_SendArray，串口初始化、收发或中断处理函数。 */
void Usart_SendArray( uint32_t, uint8_t *, uint16_t );				//8λ
/* 函数声明说明：Usart_SendString，串口初始化、收发或中断处理函数。 */
void Usart_SendString( uint32_t , char *);											//ַ
/* 函数声明说明：Usart_SendHalfWord，串口初始化、收发或中断处理函数。 */
void Usart_SendHalfWord( uint32_t , uint16_t);									//һ16λ
//int fputc(int, FILE *);																								//ضc⺯printfڣضʹprintf
//int fgetc(FILE *);																										//ضc⺯scanfڣдʹscanfgetcharȺ
#endif


