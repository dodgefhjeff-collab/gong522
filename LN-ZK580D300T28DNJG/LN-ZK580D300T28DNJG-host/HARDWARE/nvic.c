/* 文件：nvic.c，GD32C103CBT6 中断配置和服务函数 */

#include "nvic.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "delay.h"
#include "power.h"
#include <string.h>

/* 明确声明，避免工程里存在多个usart.h路径时出现隐式声明告警。 */
/* 函数声明说明：uart_rx_byte_process，USART1接收中断调用，保存PA2/PA3内部通信字节。 */
extern void uart_rx_byte_process(uint8_t ch);
/* 函数声明说明：uart2_rx_byte_process，USART2接收中断调用，完成外部SLIP帧反转义和缓存。 */
extern void uart2_rx_byte_process(uint8_t ch);

#ifndef USART_STAT

#define USART_STAT(usartx) REG32((usartx) + 0x00U)
#endif
#ifndef USART_DATA

#define USART_DATA(usartx) REG32((usartx) + 0x04U)
#endif

extern QueueHandle_t Queuehandler_USART;

extern queue_data Queue_USART;

queue_data Queue_Uart3Buff;

#if configGENERATE_RUN_TIME_STATS

extern volatile unsigned long long FreeRTOSRunTimeTicks;
#endif

/* NVIC_init_all：配置中断优先级和启用所需中断 */
void NVIC_init_all(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);

    /* 串口-DMA相关中断初始化**************************************** */
    nvic_irq_enable(USART1_IRQn, 7, 0);          // PA2/PA3内部通信口：GD32 USART1
    nvic_irq_enable(USART2_IRQn, 7, 0);          // PB10/PB11对外通信口：GD32 USART2
    nvic_irq_enable(DMA0_Channel1_IRQn, 9, 0);   // USART2_TX DMA0_CH1，当前对外发送默认阻塞，保留DMA中断
    nvic_irq_enable(DMA0_Channel6_IRQn, 9, 0);   // USART1_TX DMA0_CH6，PA2内部通信口
    nvic_irq_enable(DMA0_Channel0_IRQn, 7, 0);   // ADC0 DMA0_CH0
    nvic_irq_enable(TIMER3_IRQn, 5, 1);          // TIMER3，原工程TIM4运行统计
}

/* 中断服务函数****************************************** */
/* TIMER3_IRQHandler：定时器或PWM配置/处理函数 */
void TIMER3_IRQHandler(void)
{
    if (timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP) != RESET)
    {
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
#if configGENERATE_RUN_TIME_STATS
        FreeRTOSRunTimeTicks++;
#endif
    }
}

/* DMA0_Channel0_IRQHandler：DMA初始化或中断处理函数 */
void DMA0_Channel0_IRQHandler(void)
{
    if (dma_interrupt_flag_get(DMA0, DMA_CH0, DMA_INT_FLAG_FTF) != RESET)
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH0, DMA_INT_FLAG_G);
    }
}

/* DMA0_Channel1_IRQHandler：DMA初始化或中断处理函数 */
void DMA0_Channel1_IRQHandler(void)
{
    if (dma_interrupt_flag_get(DMA0, DMA_CH1, DMA_INT_FLAG_FTF) != RESET)
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH1, DMA_INT_FLAG_G);
        dma_channel_disable(DMA0, DMA_CH1);
    }
}

/* DMA0_Channel6_IRQHandler：DMA初始化或中断处理函数 */
void DMA0_Channel6_IRQHandler(void)
{
    if (dma_interrupt_flag_get(DMA0, DMA_CH6, DMA_INT_FLAG_FTF) != RESET)
    {
        dma_interrupt_flag_clear(DMA0, DMA_CH6, DMA_INT_FLAG_G);
        dma_channel_disable(DMA0, DMA_CH6);
    }
}

void USART1_IRQHandler(void)
{
    
    uint8_t ch;

    /* PA2/PA3 双MCU内部通信口：GD32C103库中对应USART1。 */
    if (usart_interrupt_flag_get(POWER_UART1, USART_INT_FLAG_RBNE) != RESET)
    {
        ch = (uint8_t)usart_data_receive(POWER_UART1);
        uart_rx_byte_process(ch);
    }

#ifdef USART_INT_FLAG_ERR_ORERR
    if (usart_interrupt_flag_get(POWER_UART1, USART_INT_FLAG_ERR_ORERR) != RESET)
    {
        (void)USART_STAT(POWER_UART1);
        (void)USART_DATA(POWER_UART1);
    }
#endif
}

void USART2_IRQHandler(void)
{
    
    uint8_t ch;

    /* PB10/PB11 对外通信口：GD32C103库中对应USART2。；中断里只读取数据并送入SLIP反转义状态机，不调用FreeRTOS队列。 */
    if (usart_interrupt_flag_get(POWER_UART2, USART_INT_FLAG_RBNE) != RESET)
    {
        ch = (uint8_t)usart_data_receive(POWER_UART2);
        uart2_rx_byte_process(ch);
    }

    /* 如果发生溢出/噪声错误，读状态和数据寄存器清除，防止后续RBNE不再触发。 */
#ifdef USART_INT_FLAG_ERR_ORERR
    if (usart_interrupt_flag_get(POWER_UART2, USART_INT_FLAG_ERR_ORERR) != RESET)
    {
        (void)USART_STAT(POWER_UART2);
        (void)USART_DATA(POWER_UART2);
    }
#endif
}
