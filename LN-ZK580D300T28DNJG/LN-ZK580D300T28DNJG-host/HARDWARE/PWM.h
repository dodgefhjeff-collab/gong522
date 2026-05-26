#ifndef __PWM_H

#define __PWM_H

#include "sys.h"

/* 函数声明说明：TIM1_PWM_Init，定时器或PWM配置/处理函数。 */
void TIM1_PWM_Init(uint16_t arr, uint16_t psc);
/* 函数声明说明：TIM4_PWM_Init，定时器或PWM配置/处理函数。 */
void TIM4_PWM_Init(uint16_t arr, uint16_t psc);

#endif
