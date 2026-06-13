#ifndef __GPIO_H

#define __GPIO_H

#include "sys.h"

#define EN_12V_KZ_OFF    gpio_bit_set(GPIOA, GPIO_PIN_15)
#define EN_12V_KZ_ON   gpio_bit_reset(GPIOA, GPIO_PIN_15)

#define EN_F28V_KZ_OFF   gpio_bit_set(GPIOB, GPIO_PIN_4)
#define EN_F28V_KZ_ON  gpio_bit_reset(GPIOB, GPIO_PIN_4)

#define EN_28V_KZ_OFF    gpio_bit_set(GPIOB, GPIO_PIN_3)
#define EN_28V_KZ_ON   gpio_bit_reset(GPIOB, GPIO_PIN_3)

#define LED_R2_ON       gpio_bit_set(GPIOB, GPIO_PIN_8)
#define LED_R2_OFF      gpio_bit_reset(GPIOB, GPIO_PIN_8)

#define LED_G2_ON       gpio_bit_set(GPIOB, GPIO_PIN_7)
#define LED_G2_OFF      gpio_bit_reset(GPIOB, GPIO_PIN_7)

#define LED_R1_ON       gpio_bit_set(GPIOB, GPIO_PIN_6)
#define LED_R1_OFF      gpio_bit_reset(GPIOB, GPIO_PIN_6)

#define LED_G1_ON       gpio_bit_set(GPIOB, GPIO_PIN_5)
#define LED_G1_OFF      gpio_bit_reset(GPIOB, GPIO_PIN_5)

#define G_PWM_ON       gpio_bit_set(GPIOA, GPIO_PIN_11)
#define G_PWM_OFF      gpio_bit_reset(GPIOA, GPIO_PIN_11)

/* Vopen：PA12浮空输入，低电平=外部允许输出，高电平=禁止输出 */
#define VOPEN_IS_ACTIVE()   (gpio_input_bit_get(GPIOA, GPIO_PIN_12) == RESET)

/* SRDD：PB0推挽输出，由软件根据输入电压控制；<210V输出高，>=210V输出低 */
#define SRDD_HIGH           gpio_bit_set(GPIOA, GPIO_PIN_1)
#define SRDD_LOW            gpio_bit_reset(GPIOA, GPIO_PIN_1)

#define GZ_OFF       gpio_bit_set(GPIOA, GPIO_PIN_8)
#define GZ_ON        gpio_bit_reset(GPIOA, GPIO_PIN_8)
#define GZ2_LED2_RUN_TOGG GPIO_TogglePin(GPIOA, GPIO_PIN_8)
/* 函数声明说明：GPIO_TogglePin，GPIO初始化或操作函数。 */
void GPIO_TogglePin(uint32_t gpio_periph, uint32_t gpio_pin);
/* 函数声明说明：GPIO_init_all，初始化所有GPIO方向、复用和默认输出状态。 */
void GPIO_init_all(void);
/* 根据Vopen/SRDD与协议状态刷新12V/28V/风机使能 */
void Power_UpdateHostControl(void);

#endif
