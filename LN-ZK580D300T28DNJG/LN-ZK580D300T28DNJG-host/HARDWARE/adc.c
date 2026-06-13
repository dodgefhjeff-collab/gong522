/* 文件：adc.c，GD32C103CBT6 ADC0 + DMA0_CH0扫描采样 */

#include <stdio.h>
#include <math.h>
#include "adc.h"
#include "delay.h"
#include "usart.h"
#include "gpio.h"
#include "nvic.h"

/* -------------------------------- 变 量 申 明 -------------------------------- */
uint16_t AD_Value[N][M];

uint16_t Average[M];

uint16_t Average_filter[M];

FilterChannel channels[CHANNELS];

/* Adc_Init：ADC采样、滤波或数据换算函数 */
void Adc_Init(void)
{
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);   // 72M/6=12MHz

    adc_deinit(ADC0);
    adc_mode_config(ADC_MODE_FREE);
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, M);
	
	
//		gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_1);  //温度 -PB1  
//		gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  //12V_I - PB0
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_7);  //12V_U - PA7
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_6);  //28VI  - PA6
//		gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_5);  //28VU  - PA5
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);  //VINI  - PA4
//		gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_0);  //VINU  - PA0
	
	
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5); // PB1 - 温度
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_8, ADC_SAMPLETIME_55POINT5); //12V_I - PB0
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_7, ADC_SAMPLETIME_55POINT5);//12V_U - PA7
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_6, ADC_SAMPLETIME_55POINT5); //28VI  - PA6
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5); //28VU  - PA5
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_4, ADC_SAMPLETIME_55POINT5); //VINI  - PA4
    adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5); //VINU  - PA0

    adc_dma_mode_enable(ADC0);
    adc_enable(ADC0);
    delay_ms(1);
    adc_calibration_enable(ADC0);

    dma_channel_enable(DMA0, DMA_CH0);
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}

/* Get_Adc：ADC采样、滤波或数据换算函数 */
uint16_t Get_Adc(uint8_t ch)
{
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, DISABLE);
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
    adc_regular_channel_config(ADC0, 0, ch, ADC_SAMPLETIME_1POINT5);
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

    while (adc_flag_get(ADC0, ADC_FLAG_EOC) == RESET)
        ;

    return adc_regular_data_read(ADC0);
}

/* Get_Adc_Average：ADC采样、滤波或数据换算函数 */
uint16_t Get_Adc_Average(uint8_t ch, uint8_t times)
{
    
    uint32_t temp_val = 0;
    
    uint8_t t;
    for (t = 0; t < times; t++)
    {
        temp_val += Get_Adc(ch);
    }
    return temp_val / times;
}

/* compute_Voltage：ADC采样、滤波或数据换算函数 */
void compute_Voltage(void)
{
    
    static uint8_t adc_i = 0, adc_j = 0;
    
    uint32_t SUM;

    adc_i = 0;
    adc_j = 0;
    for (adc_j = 0; adc_j < M; adc_j++)
    {
        SUM = 0;
        for (adc_i = 0; adc_i < N; adc_i++)
        {
            SUM += AD_Value[adc_i][adc_j];
        }
        Average[adc_j] = SUM / N;
        SUM = 0;
    }
}

/* 温度转换已改用 ntc_temp_table.h 中的 static inline Temp_transition/Temp_transition_int，；ADC.c/ADC.h 不再定义同名函数，避免与 NTC 查表头文件冲突。 */

/* Filter_Init：ADC采样、滤波或数据换算函数 */
void Filter_Init(void)
{
    uint8_t i, j;
    for (i = 0; i < CHANNELS; i++)
    {
        channels[i].filtered_value = 0;
        channels[i].index = 0;
        for (j = 0; j < FILTER_DEPTH; j++)
        {
            channels[i].history[j] = 0;
            channels[i].weights[j] = (j + 1) * 128 / FILTER_DEPTH;
        }
    }
}

/* Weighted_Moving_Average：ADC采样、滤波或数据换算函数 */
void Weighted_Moving_Average(uint16_t *new_data, uint16_t *filtered_data)
{
    
    uint32_t sum = 0;
    
    uint32_t weight_sum = 0;
    uint8_t i, j;
    
    uint16_t index;

    for (i = 0; i < CHANNELS; i++)
    {
        sum = 0;
        weight_sum = 0;
        channels[i].history[channels[i].index] = new_data[i];

        for (j = 0; j < FILTER_DEPTH; j++)
        {
            index = (channels[i].index + j + 1) % FILTER_DEPTH;
            sum += channels[i].history[index] * channels[i].weights[j];
            weight_sum += channels[i].weights[j];
        }

        channels[i].filtered_value = sum / weight_sum;
        filtered_data[i] = (uint16_t)(channels[i].filtered_value);
        channels[i].index = (channels[i].index + 1) % FILTER_DEPTH;
    }
}
