#ifndef __SPI_H
#define __SPI_H
/*============================= 头文件包含 =================================*/
#include "gd32c10x.h"

/*============================= 函数声明 =================================*/
void spi1_init(void);			 //初始化SPI口
void spi1_setspeed(uint8_t SpeedSet); //设置SPI速度
uint8_t spi1_readwrite_byte(uint8_t TxData);//SPI总线读写一个字节

#endif
