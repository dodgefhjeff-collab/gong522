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
		
		gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);  //温度 -PB1  
		gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  //12V_I - PB0
		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  //12V_U - PA7
		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  //28VI  - PA6
		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);  //28VU  - PA5
		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);  //VINI  - PA4
		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  //VINU  - PA0
    /* PA1：SRDD 推挽输出，由Power_UpdateSRDD()根据输入电压动态控制 */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
    SRDD_LOW;   /* 上电默认低电平（输入电压未知，先置低） */

    /* SPI相关GPIO初始化**************************************** */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);

    /* 控制/LED相关GPIO初始化**************************************** */
    // 禁用JTAG，保留SWD，释放PA15/PB3/PB4
    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);

    // PA15 -- 12V_KZ*
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15);  //12VKZ--PA15


	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);  //28VKZ--PB3
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4);  //F28VKZ--PB4
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5);  //LED_G1VKZ--PB5
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  //12VKZ--PB6
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  //12VKZ--PB7
  gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);  //12VKZ--PB8	

    /* Vopen：光耦输出到MCU，读入判断外部是否允许功率输出 */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_12);//VOPEN - PA12

    /* G-PWM：风机PWM通路使能（输出） */
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
			
			//GZ--PA8对应运行灯
			gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8); //GZ -- LED1--PA8

    /* 上电安全默认：所有输出关断，由业务逻辑（Power_UpdateHostControl等）按实际状态开启 */
    EN_12V_KZ_OFF;
    EN_28V_KZ_OFF;
    EN_F28V_KZ_OFF;
    G_PWM_OFF;
    GZ_OFF;
    LED_R1_OFF;
    LED_R2_OFF;
    LED_G1_OFF;
    LED_G2_OFF;
}

/*******************************************************************************
** 函数名称: Power_UpdateHostControl
** 功能描述: 根据Vopen电平控制12V和28V（不含风机28V）输出使能。
**           Vopen低电平=外部允许输出，高电平=禁止输出。
**           周期调用（建议1秒以内），实时跟随Vopen状态。
********************************************************************************/
void Power_UpdateHostControl(void)
{
    if (VOPEN_IS_ACTIVE())
    {
        EN_12V_KZ_ON;
        EN_28V_KZ_ON;
    }
    else
    {
        EN_12V_KZ_OFF;
        EN_28V_KZ_OFF;
    }
}


