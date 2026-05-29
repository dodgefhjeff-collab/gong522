/* 文件：dma.c，GD32C103CBT6 DMA0配置，保留原工程MYDMA_Config接口 */

#include "dma.h"
#include "adc.h"
#include "usart.h"

/* MYDMA_Config：DMA初始化或中断处理函数 */
void MYDMA_Config(void)
{
    
    dma_parameter_struct DMA_InitStructure;

    rcu_periph_clock_enable(RCU_DMA0);

    /*  */
//    /* ADC0 相关DMA配置 */
//    /*  */
//    dma_deinit(DMA0, DMA_CH0);                                // ADC0 -> DMA0_CH0
//    dma_struct_para_init(&DMA_InitStructure);
//    DMA_InitStructure.periph_addr  = (uint32_t)&ADC_RDATA(ADC0);
//    DMA_InitStructure.memory_addr  = (uint32_t)&AD_Value;
//    DMA_InitStructure.direction    = DMA_PERIPHERAL_TO_MEMORY;
//    DMA_InitStructure.number       = M * N;
//    DMA_InitStructure.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
//    DMA_InitStructure.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
//    DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
//    DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_16BIT;
//    DMA_InitStructure.priority     = DMA_PRIORITY_HIGH;
//    dma_init(DMA0, DMA_CH0, &DMA_InitStructure);
//    dma_circulation_enable(DMA0, DMA_CH0);
//    dma_memory_to_memory_disable(DMA0, DMA_CH0);

    /*  */
    /* USART1_TX DMA配置：PA2内部通信口 */
    /*  */
    dma_deinit(DMA0, DMA_CH6);                                // GD32 USART1_TX -> DMA0_CH6
    dma_struct_para_init(&DMA_InitStructure);
    DMA_InitStructure.periph_addr  = (uint32_t)&USART_DATA(POWER_UART1);
    DMA_InitStructure.memory_addr  = (uint32_t)USART_TX_BUF;
    DMA_InitStructure.direction    = DMA_MEMORY_TO_PERIPHERAL;
    DMA_InitStructure.number       = USART_TX_LEN;
    DMA_InitStructure.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    DMA_InitStructure.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_8BIT;
    DMA_InitStructure.priority     = DMA_PRIORITY_LOW;
    dma_init(DMA0, DMA_CH6, &DMA_InitStructure);
    dma_circulation_disable(DMA0, DMA_CH6);
    dma_memory_to_memory_disable(DMA0, DMA_CH6);
    dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);
    dma_channel_disable(DMA0, DMA_CH6);

    /*  */
    /* USART2_TX DMA配置：PB10对外通信口 */
    /*  */
    dma_deinit(DMA0, DMA_CH1);                                // GD32 USART2_TX -> DMA0_CH1
    dma_struct_para_init(&DMA_InitStructure);
    DMA_InitStructure.periph_addr  = (uint32_t)&USART_DATA(POWER_UART2);
    DMA_InitStructure.memory_addr  = (uint32_t)USART2_TX_BUF;
    DMA_InitStructure.direction    = DMA_MEMORY_TO_PERIPHERAL;
    DMA_InitStructure.number       = USART2_TX_LEN;
    DMA_InitStructure.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    DMA_InitStructure.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    DMA_InitStructure.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    DMA_InitStructure.memory_width = DMA_MEMORY_WIDTH_8BIT;
    DMA_InitStructure.priority     = DMA_PRIORITY_MEDIUM;
    dma_init(DMA0, DMA_CH1, &DMA_InitStructure);
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);
    dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);
    dma_channel_disable(DMA0, DMA_CH1);

    /*  */
    /* USART2_RX DMA不启用 */
    /*  */
    /* 对外协议口采用RBNE中断逐字节接收SLIP变长帧。；这里显式关闭USART2_RX DMA0_CH2，避免DMA和RBNE中断同时取数导致丢字节。 */
    dma_deinit(DMA0, DMA_CH2);
    dma_channel_disable(DMA0, DMA_CH2);
}
