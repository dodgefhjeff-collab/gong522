#ifndef __NVIC_H

#define __NVIC_H

#include "sys.h"

extern uint8_t TS_EN;

extern uint8_t TIM2_STA;

/* 函数声明说明：NVIC_init_all，配置中断优先级和启用所需中断。 */
void NVIC_init_all(void);
/* 函数声明说明：SW，SW函数，保持原工程接口并完成对应模块处理。 */
void SW(void);

#endif
