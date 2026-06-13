/**
  ************************************* Copyright ******************************
  *
  *                          (C) Copyright 2023,,China.
  *                                  All Rights Reserved
  *
  *                             By(ܵӿƼ޹˾)
  *
  *
  * ļ	   : power.c
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
#include <stdio.h>
#include "usart.h"
#include "delay.h"
#include <math.h>
#include "adc.h"
#include "dma.h"
#include "nvic.H"
#include "power.h"
#include "pwm.h"
#include <FreeRTOS.h>
#include "event_groups.h"
#include "task.h"
#include "semphr.h"
#include "crc.h"
//// ⲿ¼
//extern EventGroupHandle_t EventHandle;
/* 函数声明说明：test，test函数，保持原工程接口并完成对应模块处理。 */
void test(void);


/*------------------------------------*/

/*------------------ תڲϴ ------------------
 * ADC_results滻ĹDataConversion()ÿ20msˢһΣ
 * Power_FillUploadFrame()ٽUSART1_TX_BUFͨPA2͸48PIN
 */
/* 变量说明：ADC_results，协议换算后的ADC结果数组。 */
uint16_t ADC_results[M];
/* 变量说明：value_Y，采样换算结果或ADC通道映射变量。 */
uint16_t value_Y = 0;
/* 变量说明：power_fan_switch，电源模块状态、协议缓存或统计变量。 */
uint8_t power_fan_switch = POWER_FAN_OFF;
/* 变量说明：power_fan_duty，电源模块状态、协议缓存或统计变量。 */
uint8_t power_fan_duty = 0x00;
/* 变量说明：power_clear_maxmin_flag，电源模块状态、协议缓存或统计变量。 */
uint8_t power_clear_maxmin_flag = 0;
/* 变量说明：wendu，wendu变量，用于保存当前模块运行过程中的状态或临时数据。 */
short wendu = 0;
// ¼
/* 变量说明：EventHandle，EventHandle变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern EventGroupHandle_t EventHandle;
// 
/* 变量说明：SendMessgMutex，FreeRTOS互斥量句柄变量，用于保护共享缓冲区。 */
extern xSemaphoreHandle SendMessgMutex;

/* 变量说明：fatal_error_flag，fatal_error_flag变量，用于保存当前模块运行过程中的状态或临时数据。 */
uint16_t fatal_error_flag = 0;
/* 变量说明：error_flag，error_flag变量，用于保存当前模块运行过程中的状态或临时数据。 */
uint8_t error_flag = 0;



/*******************************************************************************
** 函数名称: Power_SetFanControl
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Power_SetFanControl(uint8_t fan_switch, uint8_t duty_code, uint8_t clear_flag)
{
    /* 变量说明：pwm_value，pwm_value变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t pwm_value;

    power_fan_switch = fan_switch;
    power_fan_duty = duty_code;
    power_clear_maxmin_flag = clear_flag;

    if ((fan_switch != POWER_FAN_ON) || (duty_code > 0xA0) || ((duty_code & 0x0F) != 0x00))
    {
        power_fan_switch = POWER_FAN_OFF;
        power_fan_duty = 0x00;
        F_PWM_SetValue(0);
        return;
    }

    pwm_value = ((uint16_t)(duty_code >> 4) * POWER_PWM_PERIOD) / 10;
    F_PWM_SetValue(pwm_value);
}

/*******************************************************************************
** 函数名称: Power_HandleHostCommandFrame
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
static void Power_HandleHostCommandFrame(uint8_t *buff)
{
    if (buff[2] == POWER_CMD_TYPE_CONTROL)
    {
        Power_SetFanControl(buff[3], buff[4], buff[5]);
    }
}

/*******************************************************************************
** : Power_HandleHostCommandByte
** : ֽڽPA3յ48PIN֡ԶѰ55 AA֡ͷ66 BB֡β
** ˵    : ɹPower_HandleHostCommandFrame()Ӱϴ
********************************************************************************/
/*******************************************************************************
** 函数名称: Power_HandleHostCommandByte
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
static void Power_HandleHostCommandByte(uint8_t ch)
{
    /* 变量说明：rx_buf，rx_buf变量，用于保存当前模块运行过程中的状态或临时数据。 */
    static uint8_t rx_buf[POWER_CMD_FRAME_LEN];
    /* 变量说明：rx_cnt，rx_cnt变量，用于保存当前模块运行过程中的状态或临时数据。 */
    static uint8_t rx_cnt = 0;

    if (rx_cnt == 0)
    {
        if (ch == POWER_UPLOAD_FRAME_HEAD1)
        {
            rx_buf[rx_cnt++] = ch;
        }
        return;
    }

    if (rx_cnt == 1)
    {
        if (ch == POWER_UPLOAD_FRAME_HEAD2)
        {
            rx_buf[rx_cnt++] = ch;
        }
        else if (ch == POWER_UPLOAD_FRAME_HEAD1)
        {
            rx_cnt = 1;
            rx_buf[0] = ch;
        }
        else
        {
            rx_cnt = 0;
        }
        return;
    }

    rx_buf[rx_cnt++] = ch;
    if (rx_cnt >= POWER_CMD_FRAME_LEN)
    {
        if (rx_buf[7] == POWER_UPLOAD_FRAME_TAIL1 && rx_buf[8] == POWER_UPLOAD_FRAME_TAIL2)
        {
            Power_HandleHostCommandFrame(rx_buf);
        }
        rx_cnt = 0;
    }
}

/*******************************************************************************
** : Power_ProcessHostCommand
** : ѭ/ADCڵãȡUSART1ջепֽ
********************************************************************************/
/*******************************************************************************
** 函数名称: Power_ProcessHostCommand
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Power_ProcessHostCommand(void)
{
    /* 变量说明：ch，局部临时变量，用于循环、长度、状态或字节处理。 */
    uint8_t ch;

    while (uart1_get_rx_byte(&ch) != 0)
    {
        Power_HandleHostCommandByte(ch);
    }
}

/*******************************************************************************
** 函数名称: Power_FillU16
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
static void Power_FillU16(uint8_t *buff, uint8_t index, uint16_t data)
{
    buff[index] = (uint8_t)(data & 0xFF);
    buff[index + 1] = (uint8_t)((data >> 8) & 0xFF);
}

static void Power_SetRdSelect(uint8_t index)
{
    if ((index & 0x04U) != 0U)
    {
        FAN_RD_S2_HIGH;
    }
    else
    {
        FAN_RD_S2_LOW;
    }

    if ((index & 0x02U) != 0U)
    {
        FAN_RD_S1_HIGH;
    }
    else
    {
        FAN_RD_S1_LOW;
    }

    if ((index & 0x01U) != 0U)
    {
        FAN_RD_S0_HIGH;
    }
    else
    {
        FAN_RD_S0_LOW;
    }
}

/* bit15~bit7 对应风机1~9，0运转，1停转 */
static uint16_t Power_ReadFanStatusBits(void)
{
    uint8_t i;
    uint16_t fan_bits = 0;

    for (i = 0; i < 9U; i++)
    {
        Power_SetRdSelect(i);
        /* 等待MUX选通稳定，延时200us确保Y引脚电平可靠 */
        delay_us(200);

        /* Y高电平（SET）表示该路风机停转，置对应状态位为1 */
        if (Y_output != 0U)
        {
            fan_bits |= (uint16_t)(1U << (15U - i));
        }
    }

    return fan_bits;
}


/*******************************************************************************
** : Power_FillUploadFrame
** : F28VU١I_F1YF_PWMݴ55 AA ... 66 BB
** ͷ: 28PIN PA2 -> 48PIN PA3
********************************************************************************/
/*******************************************************************************
** 函数名称: Power_FillUploadFrame
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Power_FillUploadFrame(void)
{
    USART1_TX_BUF[0] = POWER_UPLOAD_FRAME_HEAD1;
    USART1_TX_BUF[1] = POWER_UPLOAD_FRAME_HEAD2;

    // F28VU
    Power_FillU16(USART1_TX_BUF, 2, value_F28VU);

    // ټ⣺F9-RD
    Power_FillU16(USART1_TX_BUF, 4, value_FAN_SPEED);

    // I_F1
    Power_FillU16(USART1_TX_BUF, 6, value_I_F1);

    // Y״̬
    USART1_TX_BUF[8] = (uint8_t)(value_Y & 0xFF);

    // F_PWMǰȽֵ
    Power_FillU16(USART1_TX_BUF, 9, F_PWM_Value);

    // 风机状态位图（bit15~bit7 对应风机1~9）
    Power_FillU16(USART1_TX_BUF, 11, value_S0);

    // S1
    Power_FillU16(USART1_TX_BUF, 13, value_S1);


    // S2
    Power_FillU16(USART1_TX_BUF, 15, value_S2);

    // ¶ADC
    Power_FillU16(USART1_TX_BUF, 17, value_TEMP);

    USART1_TX_BUF[19] = POWER_UPLOAD_FRAME_TAIL1;
    USART1_TX_BUF[20] = POWER_UPLOAD_FRAME_TAIL2;
}

/* ------------------------------- begin  ------------------------------- */
/**
 ** : DataConversion
 ** : ˲õݣͨѶЭת
 ** ˵: [/]
 ** ˵: None
 ** Ա: 
 ** : 2024-07-2
 **---------------------------------------------------------------------
 ** ޸Ա:
 ** ޸:
 ** ޸:
 **---------------------------------------------------------------------
 **/
/* -------------------------------- end -------------------------------- */
/*******************************************************************************
** 函数名称: DataConversion
** 功能描述: 28PIN从机将采集/状态值打包到USART1_TX_BUF上传帧。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void DataConversion()
{
    /*
     * 28PINݰԭͼ
     * ADC_F28VU     -> PA1/ADC1
     * ADC_FAN_SPEED -> PA6/ADC6F9-RDټ
     * ADC_I_F1      -> PA0/ADC0
     * Y             -> PA4״̬
     * F_PWM         -> PWMģ¼ĵǰȽֵ
     */
    value_F28VU = ADC_F28VU;
    value_FAN_SPEED = ADC_FAN_SPEED;
    value_I_F1 = ADC_I_F1;
    value_S0 = Power_ReadFanStatusBits();
    value_Y = (uint16_t)Y_output;
    value_S1 = 0;
    value_S2 = 0;
    value_TEMP = ADC_TEMP;
    wendu = (short)value_TEMP;

    // ȡ
    if (xSemaphoreTake(SendMessgMutex, portMAX_DELAY) == pdTRUE)
    {
        Power_FillUploadFrame();

        // ͷŻ
        xSemaphoreGive(SendMessgMutex);
    }
}
/* ------------------------------- begin  ------------------------------- */
/**
 ** : test
 ** : None
 ** ˵: [/]
 ** ˵: None
 ** Ա: 
 ** : 2024-07-2
 **---------------------------------------------------------------------
 ** ޸Ա:
 ** ޸:
 ** ޸:
 **---------------------------------------------------------------------
 **/
/* -------------------------------- end -------------------------------- */

/*******************************************************************************
** 函数名称: test
** 功能描述: test函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void test()
{
    printf("\r\n");
    printf("F28VU %d [%d]\r\n", value_F28VU, ADC_F28VU);
    printf("FAN_SPEED %d [%d]\r\n", value_FAN_SPEED, ADC_FAN_SPEED);
    printf("I_F1 %d [%d]\r\n", value_I_F1, ADC_I_F1);
    printf("Y %d\r\n", value_Y);
    printf("F_PWM %d\r\n", F_PWM_Value);
    printf("S0 %d [%d]\r\n", value_S0, ADC_S0);
    printf("S1 %d [%d]\r\n", value_S1, ADC_S1);
    printf("S2 %d [%d]\r\n", value_S2, ADC_S2);
    printf("TEMP %d\r\n", value_TEMP);
    printf("-----------------------ź-----------------\r\n");

    printf("-----------------------ʱź-----------------\r\n");
    printf("ϵͳʱ: %d\n", rcu_clock_freq_get(CK_SYS));
    printf("AHBʱ: %d\n", rcu_clock_freq_get(CK_AHB));
    printf("APB1ʱ: %d\n", rcu_clock_freq_get(CK_APB1));
    printf("APB2ʱ: %d\n", rcu_clock_freq_get(CK_APB2));
}

