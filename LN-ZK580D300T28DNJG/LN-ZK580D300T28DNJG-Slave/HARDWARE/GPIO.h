/**
  ************************************* Copyright ******************************
  *
  *                         (C) Copyright 2023,,China.
  *                                 All Rights Reserved
  *
  *                            By(ܵӿƼ޹˾)
  *
  *
  * ļ       : GPIO.H
  * 汾         : v1.0
  *          : 
  * ʱ         : 2023-01-29
  *          :
  * б     :
    1. ....
          <汾>:
      <޸Ա>:
        <ʱ>:
          <>:
    2. ...
  ******************************************************************************
 */
#ifndef __GPIO_H
/* 宏定义说明：__GPIO_H，头文件重复包含保护宏。 */
#define __GPIO_H
#include "gd32c10x.h"



/* 宏定义说明：GZ2_LED1，GZ2_LED1宏定义，用于保持原工程风格并集中配置相关参数。 */
#define GZ2_LED1 gpio_input_bit_get(GPIOB, GPIO_PIN_7)

/* 风机RD多路检测：S2=PA7，S1=PB0，S0=PB1，Y=PA4 */
#define FAN_RD_S2_HIGH  gpio_bit_set(GPIOA, GPIO_PIN_7)
#define FAN_RD_S2_LOW   gpio_bit_reset(GPIOA, GPIO_PIN_7)
#define FAN_RD_S1_HIGH  gpio_bit_set(GPIOB, GPIO_PIN_0)
#define FAN_RD_S1_LOW   gpio_bit_reset(GPIOB, GPIO_PIN_0)
#define FAN_RD_S0_HIGH  gpio_bit_set(GPIOB, GPIO_PIN_1)
#define FAN_RD_S0_LOW   gpio_bit_reset(GPIOB, GPIO_PIN_1)
/* 宏定义说明：Y_output，Y_output宏定义，用于保持原工程风格并集中配置相关参数。 */
#define Y_output gpio_input_bit_get(GPIOA, GPIO_PIN_4)



/* 函数声明说明：GPIO_init_all，初始化所有GPIO方向、复用和默认输出状态。 */
void GPIO_init_all(void);
/* 函数声明说明：GPIO_TogglePin，GPIO初始化或操作函数。 */
void GPIO_TogglePin(uint32_t GPIOx, uint16_t GPIO_PIN);

#endif

