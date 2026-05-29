/**
  ************************************* Copyright ******************************
  *  
  *                         (C) Copyright 2023,,China.
  *                                 All Rights Reserved
  *                              
  *                            By(ܵӿƼ޹˾)
  *                     
  *      
  * ļ       : adc.h   
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
#ifndef __ADC_H
/* 宏定义说明：__ADC_H，头文件重复包含保护宏。 */
#define __ADC_H

#include "gd32c10x.h"


/* 宏定义说明：Rp，Rp宏定义，用于保持原工程风格并集中配置相关参数。 */
#define Rp 10000            //10k
/* 宏定义说明：T2，T2宏定义，用于保持原工程风格并集中配置相关参数。 */
#define T2 298.15           //  T2,273.15+25.0
/* 宏定义说明：Bx，Bx宏定义，用于保持原工程风格并集中配置相关参数。 */
#define Bx 3435.0
/* 宏定义说明：Ka，Ka宏定义，用于保持原工程风格并集中配置相关参数。 */
#define Ka 273.15

/* 宏定义说明：ADC_DMA_SCAN，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_DMA_SCAN            //ADCDMAɨģʽ
/* 宏定义说明：N，ADC采样、通道数量或滤波深度相关宏。 */
#define N 50 //ÿͨ50Σƽ
/* 宏定义说明：M，ADC采样、通道数量或滤波深度相关宏。 */
#define M 7  //Ϊ7ͨ

/* 宏定义说明：adc1_order_num，ADC采样、通道数量或滤波深度相关宏。 */
#define adc1_order_num N
/* 宏定义说明：adc1_ch_num，ADC采样、通道数量或滤波深度相关宏。 */
#define adc1_ch_num    M

//Ȩ˲㷨
/* 宏定义说明：CHANNELS，ADC采样、通道数量或滤波深度相关宏。 */
#define CHANNELS adc1_ch_num       // ͨ
/* 宏定义说明：FILTER_DEPTH，ADC采样、通道数量或滤波深度相关宏。 */
#define FILTER_DEPTH 50 // ˲



// Ȩ˲㷨ÿͨݽṹ
typedef struct
{
    /* 变量说明：history，history变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t history[FILTER_DEPTH];  // 洢ʷ
    /* 变量说明：filtered_value，filtered_value变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint32_t filtered_value;         // 洢˲ֵʹøԱ
    /* 变量说明：index，index变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint8_t index;                   // ǰ
    /* 变量说明：weights，weights变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t weights[FILTER_DEPTH];  // ÿͨȨ
} FilterChannel;


/* 变量说明：adc1_result，adc1_result变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern uint16_t  adc1_result[adc1_ch_num];                       //adc
/* 变量说明：Average_filter，ADC加权递推滤波结果数组。 */
extern uint16_t  Average_filter[adc1_ch_num]; //˲Ľ
/* 变量说明：channels，channels变量，用于保存当前模块运行过程中的状态或临时数据。 */
extern FilterChannel channels[CHANNELS]; // ͨ˲ݽṹ


/*ADCʼ*/
/* 函数声明说明：adc_config，ADC采样、滤波或数据换算函数。 */
void adc_config(void);

/*ƽֵȡ*/
/* 函数声明说明：FilterRecursive，ADC采样、滤波或数据换算函数。 */
uint16_t FilterRecursive(uint8_t ch);
/*ڲ¶ֵȡ*/
/* 函数声明说明：Get_the_temperature_value，ADC采样、滤波或数据换算函数。 */
char Get_the_temperature_value(void);
/* 函数声明说明：Weighted_Moving_Average，ADC采样、滤波或数据换算函数。 */
void Weighted_Moving_Average(const uint16_t* new_data, uint16_t* filtered_data);
//ADCֵת
/* 函数声明说明：compute_Voltage，ADC采样、滤波或数据换算函数。 */
void compute_Voltage(void);

#ifndef ADC_DMA_SCAN
/* 函数声明说明：Get_Adc_Average，ADC采样、滤波或数据换算函数。 */
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times);
/* 函数声明说明：adc_channel_sample，ADC采样、滤波或数据换算函数。 */
uint16_t adc_channel_sample(uint8_t channel);
#endif

#endif /* ADC_H */
