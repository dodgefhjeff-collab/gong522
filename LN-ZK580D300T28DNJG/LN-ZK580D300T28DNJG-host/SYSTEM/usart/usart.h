#ifndef __USART_H

#define __USART_H

#include "sys.h"
#include <stdio.h>
#include <stdint.h>

/* 缓存长度配置**************************************** */

#define USART_REC_LEN               64      // USART1内部通信接收缓冲，PA2/PA3内部帧最长21字节，64字节足够

#define USART_TX_LEN                32      // USART1内部通信发送缓冲，48PIN下发28PIN控制帧为9字节

/* 0x040E在线升级数据包协议长度为1064字节：帧头6 + 消息体1056 + CRC2。；中断接收时已经完成SLIP反转义，缓存这里保存“未带0xC0帧头帧尾”的原始协议帧。 */

#define UART_UPDATE_FRAME_TOTAL_LEN 1064

#define UART_FRAME_MAX_LEN          UART_UPDATE_FRAME_TOTAL_LEN

#define USART2_REC_LEN              UART_FRAME_MAX_LEN

#define USART2_TX_LEN               16      // 对外协议发送走uart_slip_tx_buff，本缓存仅保留兼容

#define USART_REC_IDLE_LEN          USART_REC_LEN

#define USART2_REC_IDLE_LEN         USART2_REC_LEN

/* 本板应答最大发送帧为0x0412：88字节，预留到96字节；SLIP最坏情况按2倍计算。 */

#define UART_TX_FRAME_MAX_LEN       96

#define UART_SLIP_TX_MAX_LEN        (UART_TX_FRAME_MAX_LEN * 2 + 4)

/* SLIP定义**************************************** */

#define UART_SLIP_END               0xC0

#define UART_SLIP_ESC               0xDB

#define UART_SLIP_ESC_END           0xDC

#define UART_SLIP_ESC_ESC           0xDD

/* 协议错误码**************************************** */

#define UART_PROTOCOL_OK            0x00

#define UART_PROTOCOL_ERR_CRC       0x01

#define UART_PROTOCOL_ERR_LEN       0x02

#define UART_PROTOCOL_ERR_ID        0x03

#define UART_PROTOCOL_ERR_DATA      0x04

#define UART_PROTOCOL_ERR_DEVICE    0x05

/* CRC两个字节是否高字节在前。；通信协议3.1要求所有2/4/8字节多字节数据大端，CRC字段也按高字节在前处理。 */

#define UART_CRC_HIGH_BYTE_FIRST    1

#define POWER_UART1                 USART1      /* PA2=TX, PA3=RX，双MCU内部通信口 */
#define POWER_UART2                 USART2      /* PB10=TX, PB11=RX，对外通信/协议口 */
#define POWER_LINK_UART_INDEX       1           /* 内部通信口编号：USART1 */
#define POWER_EXT_UART_INDEX        2           /* 对外协议口编号：USART2 */
#define DEBUG_USARTx                POWER_UART2

/* 某些GD32C10x库版本没有暴露USART_STAT/USART_DATA宏，错误清除需要直接读寄存器。 */
#ifndef USART_STAT

#define USART_STAT(usartx)          REG32((usartx) + 0x00U)
#endif
#ifndef USART_DATA

#define USART_DATA(usartx)          REG32((usartx) + 0x04U)
#endif

#define DEBUG_USART2_TX_GPIO_PORT   GPIOB

#define DEBUG_USART2_TX_GPIO_PIN    GPIO_PIN_10

#define DEBUG_USART2_RX_GPIO_PORT   GPIOB

#define DEBUG_USART2_RX_GPIO_PIN    GPIO_PIN_11

extern uint8_t USART_RX_BUF[USART_REC_IDLE_LEN];

extern uint8_t USART_TX_BUF[USART_TX_LEN];

extern uint8_t USART2_RX_BUF[USART2_REC_IDLE_LEN];

extern uint8_t USART2_TX_BUF[USART2_TX_LEN];

extern uint8_t USART_RX_STA;

extern uint8_t USART2_RX_STA;

#define Slave_RX_BUF                USART2_RX_BUF

#define Slave_TX_BUF                USART2_TX_BUF

#define Slave_RX_STA                USART2_RX_STA

/* 函数声明说明：uart_init，初始化PA2/PA3对应的USART1，用于两个单片机之间通信。 */
void uart_init(uint32_t bound);
/* 函数声明说明：uart2_init，初始化PB10/PB11对应的USART2，用于外部管理协议通信。 */
void uart2_init(uint32_t bound);
/* 函数声明说明：uart2_force_gpio_test，串口初始化、收发或中断处理函数。 */
void uart2_force_gpio_test(void);

/* 函数声明说明：uart_rx_byte_process，USART1接收中断调用，保存PA2/PA3内部通信字节。 */
void uart_rx_byte_process(uint8_t ch);
/* 函数声明说明：uart_get_rx_byte，从PA2/PA3内部通信环形缓冲读取1字节。 */
uint8_t uart_get_rx_byte(uint8_t *ch);
/* 函数声明说明：uart_clear_rx_buff，串口初始化、收发或中断处理函数。 */
void uart_clear_rx_buff(void);
/* 函数声明说明：uart_send_data，通过PA2/PA3内部通信口发送原始数据。 */
void uart_send_data(uint8_t *buff, uint16_t len);

/* 函数声明说明：uart2_rx_byte_process，USART2接收中断调用，完成外部SLIP帧反转义和缓存。 */
void uart2_rx_byte_process(uint8_t ch);
/* 函数声明说明：uart2_get_rx_frame，串口初始化、收发或中断处理函数。 */
uint8_t uart2_get_rx_frame(uint8_t *rx_buff, uint16_t *rx_len);
/* 函数声明说明：PB10_PB11_USART2_BlockTest，PB10_PB11_USART2_BlockTest函数，保持原工程接口并完成对应模块处理。 */
void PB10_PB11_USART2_BlockTest(void);
/* 函数声明说明：PB10_PB11_USART2_SendText，PB10_PB11_USART2_SendText函数，保持原工程接口并完成对应模块处理。 */
void PB10_PB11_USART2_SendText(const char *str);

/* 函数声明说明：usart_Sen_dma，usart_Sen_dma函数，保持原工程接口并完成对应模块处理。 */
void usart_Sen_dma(uint8_t uartx, uint8_t *buff, uint16_t len);
/* 函数声明说明：Usart_SendByte，串口初始化、收发或中断处理函数。 */
void Usart_SendByte(uint32_t usartx, uint8_t ch);
/* 函数声明说明：Usart_SendArray，串口初始化、收发或中断处理函数。 */
void Usart_SendArray(uint32_t usartx, uint8_t *array, uint16_t num);
/* 函数声明说明：Usart_SendString，串口初始化、收发或中断处理函数。 */
void Usart_SendString(uint32_t usartx, char *str);
/* 函数声明说明：Usart_SendHalfWord，串口初始化、收发或中断处理函数。 */
void Usart_SendHalfWord(uint32_t usartx, uint16_t ch);

/* 函数声明说明：uart_read_u16_be，串口初始化、收发或中断处理函数。 */
uint16_t uart_read_u16_be(uint8_t *buff);
/* 函数声明说明：uart_write_u16_be，串口初始化、收发或中断处理函数。 */
void uart_write_u16_be(uint8_t *buff, uint16_t data);
/* 函数声明说明：uart_crc16_8005，串口初始化、收发或中断处理函数。 */
uint16_t uart_crc16_8005(uint8_t *data, uint16_t length);
/* 函数声明说明：uart_protocol_unpack，串口初始化、收发或中断处理函数。 */
uint8_t uart_protocol_unpack(uint8_t *rx_buff, uint16_t rx_len, uint8_t *frame_buff, uint16_t *frame_len);
/* 函数声明说明：uart_send_protocol_frame，按ICD要求补长度/CRC、SLIP转义后发送外部协议帧。 */
uint8_t uart_send_protocol_frame(uint8_t uartx, uint8_t *frame_buff, uint16_t frame_len);

/* 函数声明说明：fputc，fputc函数，保持原工程接口并完成对应模块处理。 */
int fputc(int, FILE *);
/* 函数声明说明：fgetc，fgetc函数，保持原工程接口并完成对应模块处理。 */
int fgetc(FILE *);

#endif
