#ifndef __ADC_H

#define __ADC_H

#include "sys.h"

#define Rp 10000

#define T2 298.15

#define Bx 3435.0

#define Ka 273.15

#define N 50

#define M 7

#define adc_ref 3.3

#define CHANNELS M

#define FILTER_DEPTH 50

#define ADC_TEMP   Average_filter[0]

#define ADC_12V_I  Average_filter[1]

#define ADC_12V_U  Average_filter[2]

#define ADC_28V_I  Average_filter[3]

#define ADC_28V_U  Average_filter[4]

#define ADC_VIN_I  Average_filter[5]

#define ADC_VIN_U  Average_filter[6]

extern uint16_t AD_Value[N][M];

extern uint16_t Average[M];

extern uint16_t Average_filter[M];

typedef struct
{
    
    uint16_t history[FILTER_DEPTH];
    
    uint32_t filtered_value;
    
    uint8_t index;
    
    uint16_t weights[FILTER_DEPTH];
} FilterChannel;

/* 函数声明说明：Adc_Init，ADC采样、滤波或数据换算函数。 */
extern void Adc_Init(void);
/* 函数声明说明：Get_Adc，ADC采样、滤波或数据换算函数。 */
uint16_t Get_Adc(uint8_t ch);
/* 函数声明说明：Get_Adc_Average，ADC采样、滤波或数据换算函数。 */
uint16_t Get_Adc_Average(uint8_t ch, uint8_t times);
/* 函数声明说明：compute_Voltage，ADC采样、滤波或数据换算函数。 */
void compute_Voltage(void);
/* 函数声明说明：Filter_Init，ADC采样、滤波或数据换算函数。 */
void Filter_Init(void);
/* 函数声明说明：Weighted_Moving_Average，ADC采样、滤波或数据换算函数。 */
void Weighted_Moving_Average(uint16_t *new_data, uint16_t *filtered_data);

#endif
