#include "my_printf.h"
#include <string.h>
#include "delay.h"
#define buffer_size 1000            //�����С����ע���ջ��С
#ifdef FREERTOS_CONFIG_H
extern xSemaphoreHandle SemaphoerUsartTxHandle;	// �ź������
#endif


//�����GD32����STM32��GD32 USART �Ǵ�USART0��ʼ ��STM32�Ǵ� USART1��ʼ���������ǿ����ж�USART0����֪����GD32����STM32
#if defined(USART0)        //GD32

#define HAL_MAX_DELAY      0xFFFFFFFFU
#ifdef FREERTOS_CONFIG_H
//���崫��ṹ�塣��Գ�Ա��ʼ��
UART_Transmit_Context uart_context;
#endif
UART_HandleTypeDef huart0;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
uint8_t UART_TC_FLAG=0;
// �ж��Ƿ�������DMA
uint8_t isDmaEnabled(UART_HandleTypeDef *huart)
{
    // ��鴮�����ýṹ���е�DMA��ر�־λ���Ա����
    return huart->Instance->CTL2 & USART_CTL2_DENT;
}
//���ں�dma�ṹ���ʼ��
void usart_handle_init()
{
    huart0.Instance=(USART_TypeDef*)USART0;
    huart0.hdmatx=(DMA_Stream_TypeDef*)0x40020044;

    huart1.Instance=(USART_TypeDef*)USART1;
    huart1.hdmatx=(DMA_Stream_TypeDef*)0x40020080;

    huart2.Instance=(USART_TypeDef*)USART2;
    huart2.hdmatx=(DMA_Stream_TypeDef*)0X4002001C;
}

uint8_t HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
    if ((pData == NULL) || (Size == 0U))
    {
      return 1;
    }

    huart->hdmatx->CNT=Size;
    huart->hdmatx->MAR=(uint32_t)pData;
    huart->hdmatx->CTL |=DMA_CHXCTL_CHEN;

    return 0;
}
uint8_t HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    if ((pData == NULL) || (Size == 0U))
    {
      return  HAL_ERROR;
    }
    while(Size>0U)
    {
       while ((huart->Instance->STAT0 & USART_STAT0_TC) == 0U);     //�ȴ��������
       huart->Instance->STAT0&=~(USART_STAT0_TC);                   //����λ
       huart->Instance->DATA=USART_DATA_DATA & (uint32_t)*pData++;     //��������(uint32_t)pData
       Size--;
    }
    return HAL_OK;
}

// �� FreeRTOS ��ʹ�� DMA ���з���������
int my_printf(UART_HandleTypeDef *huart, const char *format, ...) 
{
int result = 0;
char buffer[buffer_size]; // ���Ը�����Ҫ������������С
#ifdef FREERTOS_CONFIG_H
// ����ȴ�ʱ��
//uint32_t baudrate = uart_context.huart->Init.BaudRate; // ��ȡ������
//uint32_t timeout_ticks = 0;
#endif
va_list args;
va_start(args, format);

// ʹ��vsnprintf���и�ʽ��
vsnprintf(buffer, sizeof(buffer), format, args);
va_end(args); // �����ɱ�����б�
// �����Ƿ�ʹ��FreeRTOSѡ��ͬ�Ĵ��䷽ʽ
#ifdef FREERTOS_CONFIG_H
//timeout_ticks = (40 * strlen(buffer)) / baudrate;			// 1/������*10bit*����*4������
if(isDmaEnabled(huart))	//�Ƿ�ʹ��DMA
{
if (HAL_UART_Transmit_DMA(huart, (uint8_t *)buffer, strlen(buffer)) == HAL_OK)
{
    // �ȴ���������ź���
    if (xSemaphoreTake(*uart_context.txSemaphore, portMAX_DELAY) == pdTRUE)
    {
        result = strlen(buffer);
    }
    else
    {
        result=-1;
    }
}
else
{
    result=-1;
}
}
else
{
if(HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY)==HAL_OK)
{
    result = strlen(buffer);
}
else
{
    result=-1;
}
}
#else
if(isDmaEnabled(huart))	//�Ƿ�ʹ��DMA
{
    while(UART_TC_FLAG);
    UART_TC_FLAG=1;
    
    if (HAL_UART_Transmit_DMA(huart, (uint8_t *)buffer, strlen(buffer)) == HAL_OK  )
    {
        result = strlen(buffer);
    }
    else
    {
        result=-1;
    }
    delay_ms(2);
}
else
{
    // ��������
    if(HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY)==HAL_OK)
    {
        result = strlen(buffer);
    }
    else
    {
        result=-1;
    }
}

#endif
//delay_ms(2);
return result;

}
#else		//STM32
#ifdef FREERTOS_CONFIG_H

//���崫��ṹ�塣��Գ�Ա��ʼ��
UART_Transmit_Context uart_context;
#endif

// �ж��Ƿ�������DMA
uint8_t isDmaEnabled(UART_HandleTypeDef *huart) {
    // ��鴮�����ýṹ���е�DMA��ر�־λ���Ա����
    return huart->Instance->CR3 & USART_CR3_DMAT;
}


#ifdef FREERTOS_CONFIG_H

// ���ڴ�����ɵĻص�����
    void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
        if (huart == uart_context.huart) {
            // �ͷ��ź�������ʾ�������
            xSemaphoreGiveFromISR(uart_context.txSemaphore, pdFALSE);
        }
    }
#endif

// �� FreeRTOS ��ʹ�� DMA ���з���������
int my_printf(UART_HandleTypeDef *huart, const char *format, ...) {
    int result = 0;
    char buffer[buffer_size]; // ���Ը�����Ҫ������������С
#ifdef configUSE_FREERTOS
    // ����ȴ�ʱ��
    //uint32_t baudrate = uart_context.huart->Init.BaudRate; // ��ȡ������
    //uint32_t timeout_ticks = 0;
#endif
    va_list args;
    va_start(args, format);

    // ʹ��vsnprintf���и�ʽ��
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args); // �����ɱ�����б�



    // �����Ƿ�ʹ��FreeRTOSѡ��ͬ�Ĵ��䷽ʽ
#ifdef FREERTOS_CONFIG_H
    // timeout_ticks = (20 * strlen(buffer)) / baudrate;			// 1/������*10bit*����*2������
     if(isDmaEnabled(huart))	//�Ƿ�ʹ��DMA
             {
                     if (HAL_UART_Transmit_DMA(huart, (uint8_t *)buffer, strlen(buffer)) == HAL_OK)
                     {
                     // �ȴ���������ź���
                     if (xSemaphoreTake(uart_context.txSemaphore, portMAX_DELAY) == pdTRUE)
                     {
                         result = strlen(buffer);
                     }
                     else
                     {
                         result=-1;
                     }
                     }
                     else
                     {
                         result=-1;
                     }
             }
             else
             {
                 if(HAL_UART_Transmit(huart, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY)==HAL_OK)
                 {
                     result = strlen(buffer);
                 }
                 else
                 {
                     result=-1;
                 }
             }
#else
    if (isDmaEnabled(huart))    //�Ƿ�ʹ��DMA
    {
        if (HAL_UART_Transmit_DMA(huart, (uint8_t *) buffer, strlen(buffer)) == HAL_OK)
        {
            result = strlen(buffer);
        }
        else
        {
            result = -1;
        }
    }
    else
    {
        // ��������
        if (HAL_UART_Transmit(huart, (uint8_t *) buffer, strlen(buffer), HAL_MAX_DELAY) == HAL_OK)
        {
            result = strlen(buffer);
        }
        else
        {
            result = -1;
        }
    }

#endif

    return result;
}

#endif
