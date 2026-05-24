/**
  ************************************* Copyright ****************************** 
  *
  *                          (C) Copyright 2023,,China.
  *                                  All Rights Reserved
  *                              
  *                             By(ܵӿƼ޹˾)
  *                     
  *    
  * ļ	   : GPIO.c   
  * 汾       : v1.0		
  *        : 			
  * ʱ       : 2023-01-29         
  *        :    
  * б   :  
  	1. ....
  	      <汾>: 		
      <޸Ա>:
  		  <ʱ>:
          <>:  
  	2. ...
  ******************************************************************************
 */
#include "gd32c10x.h"
#include "GPIO.H"

/*******************************************************************************
** 函数名称: GPIO_TogglePin
** 功能描述: GPIO初始化或操作函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void GPIO_TogglePin(uint32_t GPIOx, uint16_t GPIO_PIN)
{
    if(gpio_output_bit_get(GPIOx,GPIO_PIN)==SET)
    {
        gpio_bit_reset(GPIOx,GPIO_PIN);
    }
    else
    {
        gpio_bit_set(GPIOx,GPIO_PIN);
    }
}


/*******************************************************************************
** 函数名称: GPIO_init_all
** 功能描述: 初始化所有GPIO方向、复用和默认输出状态。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void  GPIO_init_all()
{
    rcu_periph_clock_enable(RCU_GPIOA); //GPIOʱӳʼ
    rcu_periph_clock_enable(RCU_GPIOB); //GPIOʱӳʼ
    rcu_periph_clock_enable(RCU_AF);

    gpio_pin_remap_config(GPIO_SWJ_SWDPENABLE_REMAP, ENABLE);  //SWJӳ䣬ͷPB3/PB4/PA15
    gpio_pin_remap_config(GPIO_USART0_REMAP, ENABLE);          //ԭUSART0ӳ

    /****************************GPIOʼ*****************************************/
    // PA2/PA328PINͨſڣGD32 USART1
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_3);

    /****************************ADC GPIOʼ*****************************************/
    // S2PA7/ADC7
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_7);

    // F9-RDPA6/ADC6ټ
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_6);

    // I_F1PA0/ADC0
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0);

    // F28VUPA1/ADC1
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);

    // ADC_TEMP/F-PWMԤPA5/ADC5
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_5);

    // S1PB0/ADC8S0PB1/ADC9
    gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_1);

    /****************************GPIO*****************************************/
    // GZ2LED1
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);

    // Y
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);
}

