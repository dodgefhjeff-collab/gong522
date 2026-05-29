/* 文件：gpio.c，GD32C103CBT6 48PIN单片机GPIO初始化 */

#include "gpio.h"

/* GPIO_TogglePin：GPIO初始化或操作函数 */
void GPIO_TogglePin(uint32_t gpio_periph, uint32_t gpio_pin)
{
    if ((GPIO_OCTL(gpio_periph) & gpio_pin) != 0x00u)
    {
        gpio_bit_reset(gpio_periph, gpio_pin);
    }
    else
    {
        gpio_bit_set(gpio_periph, gpio_pin);
    }
}

/* GPIO_init_all：初始化所有GPIO方向、复用和默认输出状态 */
void GPIO_init_all(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_USART1);
    rcu_periph_clock_enable(RCU_USART2);

    /* 串口相关GPIO初始化**************************************** */
    // 双MCU内部通信口TX -- PA2 -> GD32 USART1_TX
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    // 双MCU内部通信口RX -- PA3 -> GD32 USART1_RX
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    // 对外通信口TX -- PB10，USART2_TX
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    // 对外通信口RX -- PB11，USART2_RX
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* ADC GPIO初始化**************************************** */
    // 采样：PB1=12VI，PA7=12VU，PA6=28VI，PA5=28VU，PA4=VINI，PA0=VINU，PB1=温度
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ,
              GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_0);
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1 | GPIO_PIN_0);

    /* SPI相关GPIO初始化**************************************** */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    /* 控制/LED相关GPIO初始化**************************************** */
    // 禁用JTAG，保留SWD，释放PA15/PB3/PB4
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    // PA15 -- 12V_KZ*
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    // PB3 -- 28V_KZ*，PB4 -- F28V_KZ*，PB5/PB6/PB7/PB8 -- LED
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,
              GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8);

//    // Vopen -- PA12
			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
//    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12);

//    // G-PWM -- PA11，未确认PWM定时器映射，先配置为浮空输入避免误驱动
//    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
			
			//GZ--PA8对应运行灯
			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
			
			
			//SRDD
			gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_1);

			EN_12V_KZ_OFF;
			EN_28V_KZ_OFF;
			EN_F28V_KZ_OFF;
			G_PWM_OFF;
			VOPEN_OFF;
			GZ_OFF;
			LED_R1_OFF;
			LED_R2_OFF;
			LED_G1_OFF;
			LED_G2_OFF;
}


