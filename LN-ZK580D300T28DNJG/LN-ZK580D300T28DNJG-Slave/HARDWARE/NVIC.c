
#include <FreeRTOS.h>
#include "semphr.h"
#include "nvic.h"
#include "usart.h"
#include "iap_app.h"
#include "iap_crc.h"



// ź
/* 变量说明：SemaphoerCapOVPHandle，FreeRTOS信号量句柄变量，用于中断和任务同步。 */
extern xSemaphoreHandle SemaphoerCapOVPHandle; // ADCŹ

//о
/* 变量说明：Queuehandler_USART，FreeRTOS队列句柄变量，用于任务间传递消息。 */
extern QueueHandle_t Queuehandler_USART;
// Ϣлôָķʽ
/* 变量说明：Queue_USART，Queue_USART变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern queue_data Queue_USART;	   // ýջ


/* 变量说明：TS_EN，TS_EN变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint8_t TS_EN;  // ڲģʽ
/* 变量说明：TS_COU，TS_COU变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint8_t TS_COU; // ģʽ׼
/* 变量说明：UART_TC_FLAG，UART_TC_FLAG变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint8_t UART_TC_FLAG;
/*******************************************************************************
** 函数名称: NVIC_init_all
** 功能描述: 配置中断优先级和启用所需中断。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void NVIC_init_all()
{
    // ʹFreeRTOSƼȼ0-5 û6-15FreeRTOS
    nvic_priority_group_set(NVIC_PRIGROUP_PRE4_SUB0);
    nvic_irq_enable(ADC0_1_IRQn, 6, 0);        // ADCж
    nvic_irq_enable(DMA0_Channel3_IRQn, 9, 0); // 0 DMA  3ռȼ 0Ӧȼ
    nvic_irq_enable(DMA0_Channel6_IRQn, 10, 0); // 1 DMA  3ռȼ 1Ӧȼ
    // nvic_irq_enable(DMA0_Channel1_IRQn,3,2);                    //2 DMA  3ռȼ 2Ӧȼ
    nvic_irq_enable(USART0_IRQn, 8, 0); // 0 ȫ  0ռȼ 0Ӧȼ
    nvic_irq_enable(USART1_IRQn, 8, 1); // PA2/PA3 48PIN internal communication
    //nvic_irq_enable(USART2_IRQn,0,2);                           //1 ȫ  0ռȼ 2Ӧȼ
    nvic_irq_enable(TIMER2_IRQn, 1, 0); // ʱ4 ȫ  4ռȼ 1Ӧȼ
    // nvic_irq_enable(TIMER4_IRQn,4,2);                           //ʱ4 ȫ  4ռȼ 2Ӧȼ
    nvic_irq_enable(DMA0_Channel0_IRQn, 6, 1); // ADC0 ȫ 1ռȼ 1Ӧȼ
    nvic_irq_enable(TIMER1_IRQn, 11, 0);
}

/**********************************жϷ*******************************************/

// DMA1ch1жϷADCɼ
void DMA0_Channel0_IRQHandler(void) // DMA0ͨ0ж,ADC
{
    if (dma_flag_get(DMA0, DMA_CH0, DMA_FLAG_FTF) != RESET) // DMA0жϷ
    {
        dma_flag_clear(DMA0, DMA_CH0, DMA_FLAG_FTF); // DMA0жϱ־
        adc_disable(ADC0);                           // ʧָADC0
    }

    // DMA_Cmd(DMA1_Channel1, DISABLE);									//DMA_Mode_NormalʱҪʧDMA,ݳȡٴ
    // DMA_SetCurrDataCounter(DMA1_Channel1,N*M );			//DMA_Mode_NormalʱҪʧDMA,ݳȡٴ
}

// DMA0ch1жϷUSART2_TX
void DMA0_Channel1_IRQHandler(void) // DMA0ͨ2ж,USART2_TX
{
    if (dma_flag_get(DMA0, DMA_CH1, DMA_FLAG_FTF) != RESET) // DMA0жϷ
    {
        dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_FTF); // DMA0жϱ־
        dma_channel_disable(DMA0, DMA_CH1);
        // dma_transfer_number_config(DMA0,DMA_CH1,USART2_TX_LEN);     //ݴ䳤
    }
}

// DMA0ch6жϷUSART1_TX
void DMA0_Channel6_IRQHandler(void) // DMA0ͨ6ж,USART1_TX
{
    if (dma_flag_get(DMA0, DMA_CH6, DMA_FLAG_FTF) != RESET) // DMA0жϷ
    {
        dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_FTF); // DMA0жϱ־
        dma_channel_disable(DMA0, DMA_CH6);
        // dma_transfer_number_config(DMA0,DMA_CH6,USART1_TX_LEN);     //ݴ䳤
    }
}

// DMA0ch3жϷUSART0_TX
void DMA0_Channel3_IRQHandler(void) // DMA1ͨ3ж,USART0_TX
{
    if (dma_flag_get(DMA0, DMA_CH3, DMA_FLAG_FTF) != RESET) // DMA0жϷ
    {
        dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_FTF); // DMA0жϱ־
        dma_channel_disable(DMA0, DMA_CH3);
        UART_TC_FLAG = 0;
        // dma_transfer_number_config(DMA0,DMA_CH3,USART_TX_LEN);     //ݴ䳤
    }
}

void USART0_IRQHandler(void) // 0жϷ
{
    /* 变量说明：Res，Res变量，用于保存当前模块运行过程中的状态或临时数据。 */
    __IO uint16_t Res;
    /* 变量说明：xHPTW，xHPTW变量，用于保存当前模块运行过程中的状态或临时数据。 */
    BaseType_t xHPTW = NULL;
    /* 变量说明：len，局部临时变量，用于循环、长度、状态或字节处理。 */
    uint16_t len = 0;
    if (usart_flag_get(USART0, USART_FLAG_IDLE) != RESET)
    {
        Res = USART_STAT0(USART0);
        Res = USART_DATA(USART0); // һΡUSART_STAT0  USART_DATA

         len=USART_REC_IDLE_LEN-dma_transfer_number_get(DMA0,DMA_CH4);
        (void)len;
        //    	Usart_SendArray(USART0,USART_RX_BUF,len);

        dma_channel_disable(DMA0, DMA_CH4);
        dma_transfer_number_config(DMA0, DMA_CH4, USART_REC_IDLE_LEN);
        dma_channel_enable(DMA0, DMA_CH4);
        if ((USART_RX_BUF[0] == 0xF1) && (USART_RX_BUF[1] == 0x01))
        {
           if (check_data(USART_RX_BUF, len)) // crcУɹ
           {
              systeam_ReStart();
           }
        }
        else
        {
						Queue_USART.buff_addr=&USART1_RX_BUF[0];
						Queue_USART.buff_size=len;
						xQueueOverwriteFromISR(Queuehandler_USART,&Queue_USART,&xHPTW);
							//Ҫͽл
						portYIELD_FROM_ISR(xHPTW);
        }
    }
}

void USART1_IRQHandler(void) // USART1 PA2/PA3 internal communication
{
    /* 变量说明：ch，局部临时变量，用于循环、长度、状态或字节处理。 */
    uint8_t ch;

    if (usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE) != RESET)
    {
        ch = (uint8_t)usart_data_receive(USART1);
        uart1_rx_byte_process(ch);
    }

    if (usart_flag_get(USART1, USART_FLAG_IDLE) != RESET)
    {
        (void)USART_STAT0(USART1);
        (void)USART_DATA(USART1);
    }
}

void USART2_IRQHandler(void) // 2жϷ
{
    /* 变量说明：Res，Res变量，用于保存当前模块运行过程中的状态或临时数据。 */
    __IO uint16_t Res;
    //    uint16_t len = 0;
    if (usart_flag_get(USART2, USART_FLAG_IDLE) != RESET)
    {
        Res = USART_STAT0(USART2);
        Res = USART_DATA(USART2); // һΡUSART_STAT0  USART_DATA

        //				len=USART_REC_IDLE_LEN-dma_transfer_number_get(DMA0,DMA_CH2);
        //				Usart_SendArray(USART0,USART2_RX_BUF,len);

        dma_channel_disable(DMA0, DMA_CH2);
        dma_transfer_number_config(DMA0, DMA_CH2, USART_REC_IDLE_LEN);
        dma_channel_enable(DMA0, DMA_CH2);
    }
}

/*******************************************************************************
** 函数名称: ADC0_1_IRQHandler
** 功能描述: ADC0_1_IRQHandler函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void ADC0_1_IRQHandler(void)
{
    /* 变量说明：xHPTW，xHPTW变量，用于保存当前模块运行过程中的状态或临时数据。 */
    BaseType_t xHPTW = NULL;
    if (adc_interrupt_flag_get(ADC0, ADC_INT_FLAG_WDE) == SET) // ǲǿŹж
    {
        adc_interrupt_flag_clear(ADC0, ADC_INT_FLAG_WDE);     // ־
        xSemaphoreGiveFromISR(SemaphoerCapOVPHandle, &xHPTW); // ͷź
        portYIELD_FROM_ISR(xHPTW);                            // Ҫһл
    }
}
