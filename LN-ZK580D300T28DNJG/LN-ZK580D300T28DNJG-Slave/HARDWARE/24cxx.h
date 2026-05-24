/*
 * 24cxx.h
 *
 *  Created on: 2018-2-26
 *      Author: Administrator
 */

#ifndef _24cxx_H
#define _24cxx_H
#include "sys.h"

#include "iic.h"


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767


#define EE_TYPE     AT24C08

#define AT24CXX_WR     0       //写操作
#define AT24CXX_RD     1       //读操作
#define AT24CXX_ADDER  0XA8

unsigned char AT24CXX_ReadOneByte(uint16_t ReadAddr);							                          //指定地址读取一个字节
void AT24CXX_WriteOneByte(uint16_t WriteAddr,unsigned char DataToWrite);		              //指定地址写入一个字节
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,unsigned char Len);     //指定地址开始写入指定长度的数据
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,unsigned char Len);					                  //指定地址开始读取指定长度数据
void AT24CXX_Write(uint16_t WriteAddr,unsigned char *pBuffer,uint16_t NumToWrite);	          //从指定地址开始写入指定长度的数据
void AT24CXX_Read(uint16_t ReadAddr,unsigned char *pBuffer,uint16_t NumToRead);   	      //从指定地址开始读出指定长度的数据

unsigned char AT24CXX_Check(void);  //检查器件
void AT24CXX_Init(void);            //初始化IIC


#endif /* 24CXX_H_ */
