#ifndef __POWER_H
/* 宏定义说明：__POWER_H，头文件重复包含保护宏。 */
#define __POWER_H

#include "gd32c10x.h"
#include <stdio.h>
#include "GPIO.h"
#include "adc.h"
//#include "my_printf.h"
//---------------궨---------------//
/* 宏定义说明：ON，ON宏定义，用于保持原工程风格并集中配置相关参数。 */
#define ON  1
/* 宏定义说明：OFF，OFF宏定义，用于保持原工程风格并集中配置相关参数。 */
#define OFF 0

/*
 * 28PINͨӦϵԭͼ꣺
 *   PA7 / ADC7  -> S2
 *   PB0 / ADC8  -> S1
 *   PA0 / ADC0  -> I_F1
 *   PA6 / ADC6  -> F9-RDټ
 *   PA1 / ADC1  -> F28VU
 *   PB1 / ADC9  -> S0
 *   PA4         -> Y
 *   F-PWM       -> PWMֵPWMģ¼
 */
/* 宏定义说明：ADC_S2，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_S2          Average_filter[0]
/* 宏定义说明：ADC_S1，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_S1          Average_filter[1]
/* 宏定义说明：ADC_I_F1，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_I_F1        Average_filter[2]
/* 宏定义说明：ADC_FAN_SPEED，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_FAN_SPEED   Average_filter[3]
/* 宏定义说明：ADC_F28VU，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_F28VU       Average_filter[4]
/* 宏定义说明：ADC_S0，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_S0          Average_filter[5]
/* 宏定义说明：ADC_TEMP，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_TEMP        Average_filter[6]

/* 宏定义说明：value_S2，value_S2宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_S2            ADC_results[0]
/* 宏定义说明：value_S1，value_S1宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_S1            ADC_results[1]
/* 宏定义说明：value_I_F1，value_I_F1宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_I_F1          ADC_results[2]
/* 宏定义说明：value_FAN_SPEED，value_FAN_SPEED宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_FAN_SPEED     ADC_results[3]
/* 宏定义说明：value_F28VU，value_F28VU宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_F28VU         ADC_results[4]
/* 宏定义说明：value_S0，value_S0宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_S0            ADC_results[5]
/* 宏定义说明：value_TEMP，value_TEMP宏定义，用于保持原工程风格并集中配置相关参数。 */
#define value_TEMP          ADC_results[6]

/* 变量说明：value_Y，采样换算结果或ADC通道映射变量。 */
extern uint16_t value_Y;
/* 变量说明：power_fan_switch，电源模块状态、协议缓存或统计变量。 */
extern uint8_t power_fan_switch;
/* 变量说明：power_fan_duty，电源模块状态、协议缓存或统计变量。 */
extern uint8_t power_fan_duty;
/* 变量说明：power_clear_maxmin_flag，电源模块状态、协议缓存或统计变量。 */
extern uint8_t power_clear_maxmin_flag;

/*------------------ PA2/PA3ڲͨ֡ ------------------
 * 28PINϴ֡55 AA +  + 66 BB21ֽڡ
 * 48PIN֡55 AA C1  ռձ ־ 00 66 BB9ֽڡ
 * ע⣺MCU֮İڶ֡ʹöICDSLIPCRC
 */
/* 宏定义说明：POWER_UPLOAD_FRAME_HEAD1，电源模块业务或内部通信协议相关宏。 */
#define POWER_UPLOAD_FRAME_HEAD1    0x55
/* 宏定义说明：POWER_UPLOAD_FRAME_HEAD2，电源模块业务或内部通信协议相关宏。 */
#define POWER_UPLOAD_FRAME_HEAD2    0xAA
/* 宏定义说明：POWER_UPLOAD_FRAME_TAIL1，电源模块业务或内部通信协议相关宏。 */
#define POWER_UPLOAD_FRAME_TAIL1    0x66
/* 宏定义说明：POWER_UPLOAD_FRAME_TAIL2，电源模块业务或内部通信协议相关宏。 */
#define POWER_UPLOAD_FRAME_TAIL2    0xBB
/* 宏定义说明：POWER_UPLOAD_FRAME_LEN，电源模块业务或内部通信协议相关宏。 */
#define POWER_UPLOAD_FRAME_LEN      21
/* 宏定义说明：POWER_CMD_FRAME_LEN，电源模块业务或内部通信协议相关宏。 */
#define POWER_CMD_FRAME_LEN         9
/* 宏定义说明：POWER_CMD_TYPE_CONTROL，电源模块业务或内部通信协议相关宏。 */
#define POWER_CMD_TYPE_CONTROL      0xC1

/* 宏定义说明：POWER_FAN_OFF，风机控制相关宏。 */
#define POWER_FAN_OFF               0x00
/* 宏定义说明：POWER_FAN_ON，风机控制相关宏。 */
#define POWER_FAN_ON                0x01
/* 宏定义说明：POWER_PWM_PERIOD，电源模块业务或内部通信协议相关宏。 */
#define POWER_PWM_PERIOD            999

//---------------ṹ嶨---------------//
typedef struct
{
    /* 变量说明：EN，EN变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint8_t EN;             //ʹ            1   0ر
    /* 变量说明：SW，SW变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint8_t SW;             //ǰ״̬        1   0ر
    /* 变量说明：GY，GY变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint8_t GY;             //ǰѹ״̬        1ѹѹ 0: δѹ
    /* 变量说明：OC，OC变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t OC;            //            ѭxοʼ
    /* 变量说明：MC，MC变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t MC;            //ϼ            ѭxκʼ

}PO_InitTypeDef;

/* 变量说明：ADC_results，协议换算后的ADC结果数组。 */
extern uint16_t ADC_results[M];
/* 变量说明：fatal_error_flag，fatal_error_flag变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint16_t fatal_error_flag;
/* 变量说明：error_flag，error_flag变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint8_t error_flag;

/* 函数声明说明：test，test函数，保持原工程接口并完成对应模块处理。 */
void test(void);            //
/* 函数声明说明：DataConversion，28PIN从机将采集/状态值打包到USART1_TX_BUF上传帧。 */
void DataConversion(void);  //ת
/* 函数声明说明：Power_FillUploadFrame，电源模块业务处理函数。 */
void Power_FillUploadFrame(void);
/* 函数声明说明：Power_ProcessHostCommand，电源模块业务处理函数。 */
void Power_ProcessHostCommand(void);
/* 函数声明说明：Power_SetFanControl，电源模块业务处理函数。 */
void Power_SetFanControl(uint8_t fan_switch, uint8_t duty_code, uint8_t clear_flag);

#endif

