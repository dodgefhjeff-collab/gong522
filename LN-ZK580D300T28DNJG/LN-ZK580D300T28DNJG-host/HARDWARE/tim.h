#ifndef __TIM_H

#define __TIM_H

#include "sys.h"

/* 为兼容原工程少量TIM_SetCompare写法 */

#define TIM2 TIMER1

#define TIM3 TIMER2

#define TIM4 TIMER3

#define TIM_SetCompare1(timerx, value) timer_channel_output_pulse_value_config((timerx), TIMER_CH_0, (value))

#define TIM_SetCompare3(timerx, value) timer_channel_output_pulse_value_config((timerx), TIMER_CH_2, (value))

#define TIM_SetCompare4(timerx, value) timer_channel_output_pulse_value_config((timerx), TIMER_CH_3, (value))

/* 函数声明说明：TIM2_init_all，定时器或PWM配置/处理函数。 */
void TIM2_init_all(uint16_t arr, uint16_t psc);
/* 函数声明说明：TIM4_init_all，定时器或PWM配置/处理函数。 */
void TIM4_init_all(uint16_t arr, uint16_t psc);
/* 函数声明说明：TIM3_init_all，定时器或PWM配置/处理函数。 */
void TIM3_init_all(uint16_t arr, uint16_t psc);

#endif
