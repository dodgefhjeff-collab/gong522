#ifndef __PWM_H
/* 宏定义说明：__PWM_H，头文件重复包含保护宏。 */
#define	__PWM_H
#include "sys.h"

/* 变量说明：F_PWM_Value，风机PWM当前比较值缓存，用于28PIN上传给48PIN。 */
extern uint16_t F_PWM_Value;

/* 函数声明说明：pwm_config，定时器或PWM配置/处理函数。 */
void pwm_config(uint16_t arr,uint16_t psc);
/* 函数声明说明：F_PWM_SetValue，F_PWM_SetValue函数，保持原工程接口并完成对应模块处理。 */
void F_PWM_SetValue(uint16_t value);

#endif

