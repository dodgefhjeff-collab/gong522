/*
 * iic.h
 *
 *  Created on: 2018-2-5
 *      Author: Administrator
 */

#ifndef IIC_H_
#define IIC_H_
#include "gd32c10x.h"

#define IIC_SCL_SETH	gpio_bit_set(GPIOB,GPIO_PIN_14)         //=1
#define IIC_SCL_SETL	gpio_bit_reset(GPIOB,GPIO_PIN_14)       //=0

#define IIC_SDA_SETH	gpio_bit_set(GPIOB,GPIO_PIN_15)         //=1
#define IIC_SDA_SETL	gpio_bit_reset(GPIOB,GPIO_PIN_15)       //=0

#define READ_SDA   		gpio_input_bit_get(GPIOB,GPIO_PIN_15)

//IIC所有操作函数
void IICA_Init(void);                //初始化IIC的IO口
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(unsigned char txd);			//IIC发送一个字节
unsigned char IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
unsigned char IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号


#endif /* IIC_H_ */
