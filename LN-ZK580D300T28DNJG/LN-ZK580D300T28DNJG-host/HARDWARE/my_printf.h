
#ifndef INC_MY_PRINTF_H
#define INC_MY_PRINTF_H
//USART 结构体移植STM32
#include <stdarg.h>
#include "gd32c10x.h"
#include "stdio.h"
#include "gd32c10x_usart.h"
//#include "FreeRTOS.h"
//#include "semphr.h"

#ifndef __O
#define     __O     volatile             /*!< Defines 'write only' permissions */
#endif

#ifndef __IO
#define     __IO    volatile             /*!< Defines 'read / write' permissions */
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif


#ifdef FREERTOS_CONFIG_H
// 定义一个结构体来包含串口句柄和传输完成的信号量
typedef struct {
    UART_HandleTypeDef *huart;
    xSemaphoreHandle *txSemaphore;
} UART_Transmit_Context;

extern UART_Transmit_Context uart_context;
#endif
#if defined(USART0)        //GD32



typedef enum
{
HAL_OK       = 0x00U,
HAL_ERROR    = 0x01U,
HAL_BUSY     = 0x02U,
HAL_TIMEOUT  = 0x03U
} HAL_StatusTypeDef;


//usart寄存器结构体
typedef struct
{
__IO uint32_t STAT0;         /*!< USART Status register,                   Address offset: 0x00 */
__IO uint32_t DATA;         /*!< USART Data register,                     Address offset: 0x04 */
__IO uint32_t BAUD;        /*!< USART Baud rate register,                Address offset: 0x08 */
__IO uint32_t CTL0;        /*!< USART Control register 1,                Address offset: 0x0C */
__IO uint32_t CTL1;        /*!< USART Control register 2,                Address offset: 0x10 */
__IO uint32_t CTL2;        /*!< USART Control register 3,                Address offset: 0x14 */
__IO uint32_t GP;       /*!< USART Guard time and prescaler register, Address offset: 0x18 */
} USART_TypeDef;
//usart参数结构体
typedef struct
{
uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate.
                                           The baud rate is computed using the following formula:
                                           - IntegerDivider = ((PCLKx) / (8 * (OVR8+1) * (huart->Init.BaudRate)))
                                           - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 8 * (OVR8+1)) + 0.5
                                           Where OVR8 is the "oversampling by 8 mode" configuration bit in the CR1 register. */

uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
                                           This parameter can be a value of @ref UART_Word_Length */

uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
                                           This parameter can be a value of @ref UART_Stop_Bits */

uint32_t Parity;                    /*!< Specifies the parity mode.
                                           This parameter can be a value of @ref UART_Parity
                                           @note When parity is enabled, the computed parity is inserted
                                                 at the MSB position of the transmitted data (9th bit when
                                                 the word length is set to 9 data bits; 8th bit when the
                                                 word length is set to 8 data bits). */

uint32_t Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Mode */

uint32_t HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled or disabled.
                                           This parameter can be a value of @ref UART_Hardware_Flow_Control */

uint32_t OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
                                           This parameter can be a value of @ref UART_Over_Sampling */
} UART_InitTypeDef;



typedef struct
{
__IO uint32_t CTL;     /*!< DMA stream x 配置寄存器      */
__IO uint32_t CNT;   /*!< DMA stream x 数据寄存器数量     */
__IO uint32_t PAR;    /*!< DMA stream x 外围地址寄存器 */
__IO uint32_t MAR;   /*!< DMA stream x 存储器地址寄存器   */
//    __IO uint32_t M1AR;   /*!< DMA stream x 存储器1地址寄存器   */
//    __IO uint32_t FCR;    /*!< DMA stream x FIFO控制寄存器      */
} DMA_Stream_TypeDef;

//typedef struct
//{
//    uint32_t periph_addr;       /*!< peripheral base address */
//    uint32_t periph_width;      /*!< transfer data size of peripheral */
//    uint32_t memory_addr;       /*!< memory base address */
//    uint32_t memory_width;      /*!< transfer data size of memory */
//    uint32_t number;            /*!< channel transfer number */
//    uint32_t priority;          /*!< channel priority level */
//    uint8_t periph_inc;         /*!< peripheral increasing mode */
//    uint8_t memory_inc;         /*!< memory increasing mode */
//    uint8_t direction;          /*!< channel data transfer direction */
//
//} dma_parameter_struct;


//typedef struct __DMA_HandleTypeDef
//{
//    DMA_Stream_TypeDef         *Instance;                                                        /*!< Register base address                  */

//    //dma_parameter_struct        Init;                                                             /*!< DMA communication parameters           */

//}DMA_HandleTypeDef;

//USART句柄
typedef struct __UART_HandleTypeDef
{
USART_TypeDef                 *Instance;        /*!< UART寄存器基址        */
UART_InitTypeDef              Init;             /*!< UART通信参数      */
DMA_Stream_TypeDef            *hdmatx;          /*!< UART Tx DMA 句柄参数     */
DMA_Stream_TypeDef            *hdmarx;          /*!< UART Rx DMA 句柄参数     */
} UART_HandleTypeDef;

extern UART_HandleTypeDef huart0;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
void usart_handle_init(void);
uint8_t isDmaEnabled(UART_HandleTypeDef *huart);
uint8_t HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
uint8_t HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
int my_printf(UART_HandleTypeDef *huart, const char *format, ...);

#else

int my_printf(UART_HandleTypeDef *huart, const char *format, ...);
#endif
#endif
