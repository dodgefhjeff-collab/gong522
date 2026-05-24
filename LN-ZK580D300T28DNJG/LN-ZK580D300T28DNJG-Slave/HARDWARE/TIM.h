#ifndef __TIM_H
/* 宏定义说明：__TIM_H，头文件重复包含保护宏。 */
#define	__TIM_H
#include "sys.h"
/* 函数声明说明：TIM3_init_all，定时器或PWM配置/处理函数。 */
void TIM3_init_all(u16,u16);
/* 函数声明说明：TIM4_init_all，定时器或PWM配置/处理函数。 */
void TIM4_init_all(u16 arr,u16 psc);
#endif

