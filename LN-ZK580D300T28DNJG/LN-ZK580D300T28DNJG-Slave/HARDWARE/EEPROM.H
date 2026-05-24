#ifndef __EEPROM_H
#define	__EEPROM_H

#include "sys.h"


#define IIC_SdaModeOut()					Port_SetMode(GPIOB, GPIO_PIN_7, GPIO_MODE_OUTPUT_OD)
#define IIC_SdaModeIn()						Port_SetMode(GPIOB, GPIO_PIN_7, GPIO_MODE_INPUT)
#define IIC_SdaOutput_H()					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET)
#define IIC_SdaOutput_L()					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET)
#define IIC_SdaRead()						HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7)

#define IIC_SclModeOut()					Port_SetMode(GPIOB, GPIO_PIN_6, GPIO_MODE_OUTPUT_OD)
#define IIC_SclModeIn()						Port_SetMode(GPIOB, GPIO_PIN_6, GPIO_MODE_INPUT)
#define IIC_SclOutput_H()					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET)
#define IIC_SclOutput_L()					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET)



#define READ_CMD				1
#define WRITE_CMD				0

#define x24C08//器件名称，x24C04、x24C08或x24C16
#define DEV_ADDR				0xA0					//设备硬件地址

#ifdef x24C04
#define PAGE_NUM			32						//页数
#define PAGE_SIZE			16						//页面大小(字节)
#define CAPACITY_SIZE		(PAGE_NUM * PAGE_SIZE)	//总容量(字节)
#define ADDR_BYTE_NUM		1						//地址字节个数

#elseif x24C08
#define PAGE_NUM			64						//页数
#define PAGE_SIZE			16						//页面大小(字节)
#define CAPACITY_SIZE		(PAGE_NUM * PAGE_SIZE)	//总容量(字节)
#define ADDR_BYTE_NUM		1						//地址字节个数

#elseif x24C16
#define PAGE_NUM			128						//页数
#define PAGE_SIZE			16						//页面大小(字节)
#define CAPACITY_SIZE		(PAGE_NUM * PAGE_SIZE)	//总容量(字节)
#define ADDR_BYTE_NUM		1						//地址字节个数
#endif

#endif


