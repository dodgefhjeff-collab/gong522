#ifndef __DAC_H
#define	__DAC_H


#include "stm32f10x.h"

//DAC DHR12RD寄存器，12位、右对齐、双通道
#define DAC_DHR12RD_ADDRESS      (DAC_BASE+0x20)


extern void DAC_Mode_Init(void);
extern void DAC_out(float dac_v);

#endif /* __DAC_H */

