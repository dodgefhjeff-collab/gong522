
#include "adc.h"
#include "delay.h"
#include "math.h"
/*ADCͨ*/
#ifdef ADC_DMA_SCAN
uint16_t  adc1_buf[adc1_order_num][adc1_ch_num]={0};           //adcDMA
#else
/* 变量说明：adc1_buf，adc1_buf变量，用于保存当前模块运行过程中的状态或临时数据。 */
uint16_t  adc1_buf[adc1_ch_num]={0};           //adc
#endif
/* 变量说明：adc1_result，adc1_result变量，用于保存当前模块运行过程中的状态或临时数据。 */
uint16_t  adc1_result[adc1_ch_num]={0};                       //adc
/* 变量说明：Average_filter，ADC加权递推滤波结果数组。 */
uint16_t  Average_filter[adc1_ch_num]; //˲Ľ
/* 变量说明：channels，channels变量，用于保存当前模块运行过程中的状态或临时数据。 */
FilterChannel channels[CHANNELS]; // ͨ˲ݽṹ

/**/
/* 函数声明说明：rcu_config，rcu_config函数，保持原工程接口并完成对应模块处理。 */
void rcu_config(void);
// void gpio_config(void);
/* 函数声明说明：dma_config，dma_config函数，保持原工程接口并完成对应模块处理。 */
void dma_config(void);
/* 函数声明说明：adc_config，ADC采样、滤波或数据换算函数。 */
void adc_config(void);
// ʼ˲
/* 函数声明说明：Filter_Init，ADC采样、滤波或数据换算函数。 */
void Filter_Init(void);

//ADCֵת
/*******************************************************************************
** 函数名称: compute_Voltage
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void compute_Voltage(void)
{
    /* 变量说明：adc_i，adc_i变量，用于保存当前模块运行过程中的状态或临时数据。 */
    static uint16_t adc_i = 0, adc_j = 0;
    /* 变量说明：SUM，SUM变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint32_t SUM=0;
    adc_i=0;
    adc_j=0;
    for (adc_j = 0; adc_j < adc1_ch_num; adc_j++)
    {
        SUM = 0;
        for (adc_i = 0; adc_i < adc1_order_num; adc_i++)
        {
            SUM += adc1_buf[adc_i][adc_j];
        }
        adc1_result[adc_j] = SUM / adc1_order_num;
        SUM = 0;
    }
    Weighted_Moving_Average(adc1_result,Average_filter);

}
// ʼ˲
/*******************************************************************************
** 函数名称: Filter_Init
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Filter_Init(void)
{
    uint8_t i,j;
    for (i = 0; i < CHANNELS; i++)
    {
        channels[i].filtered_value = 0;
        channels[i].index = 0;
        for (j = 0; j < FILTER_DEPTH; j++)
        {
            channels[i].history[j] = 0;
            // ???Ȩط??
            channels[i].weights[j] = (j + 1) * 128 / FILTER_DEPTH; // ȷȨ
        }
    }
}

// Ȩ˲㷨
/*******************************************************************************
** 函数名称: Weighted_Moving_Average
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Weighted_Moving_Average(const uint16_t* new_data, uint16_t* filtered_data)
{
    /* 变量说明：sum，sum变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint32_t sum = 0;
    /* 变量说明：weight_sum，weight_sum变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint32_t weight_sum = 0;
    uint8_t i,j;
    /* 变量说明：index，index变量，用于保存当前模块运行过程中的状态或临时数据。 */
    uint16_t index;
    for (i = 0; i < CHANNELS; i++)
    {
        sum=0;
        weight_sum=0;
        // ʷ
        channels[i].history[channels[i].index] = new_data[i];

        // Ȩƽ
        for (j = 0; j < FILTER_DEPTH; j++)
        {
            index = (channels[i].index + j + 1) % FILTER_DEPTH;
            sum += channels[i].history[index] * channels[i].weights[j]; // ӦȨ
            weight_sum += channels[i].weights[j];
        }
        // ˲
        channels[i].filtered_value = sum / weight_sum;

        // תΪ16λ
        filtered_data[i] = (uint16_t)(channels[i].filtered_value);

        // 
        channels[i].index = (channels[i].index + 1) % FILTER_DEPTH;
    }
}
/*!
    \brief      ȡ¶ֵ
    \param[in]  none
    \param[out] ¶ֵ
    \retval     none
*/
/*******************************************************************************
** 函数名称: Get_the_temperature_value
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
char Get_the_temperature_value(void)
{
    /* 变量说明：temperature，temperature变量，用于保存当前模块运行过程中的状态或临时数据。 */
    char temperature = 0;
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_INSERTED_CHANNEL);
    /* value convert */
    temperature = (char)((1.43 - ADC_IDATA0(ADC0) * 3.3 / 4096) * 1000 / 4.3 + 25);
    /* 变量说明：temperature，temperature变量，用于保存当前模块运行过程中的状态或临时数据。 */
    return temperature;
}

/*!
    \brief      configure the ADC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
/*******************************************************************************
** 函数名称: adc_config
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void adc_config(void)
{
    rcu_config();
    Filter_Init();
    // gpio_config();

    //ȡIO

#ifdef ADC_DMA_SCAN
    dma_config();

    /* ADC */
    adc_deinit(ADC0);
    /* ADCģʽ */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC*/
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);
    /* ADCɨ蹦 */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);
    /* ADCݶ */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADCͨ */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, adc1_ch_num);
    /* ADCͨ */ //˴ҪõADCͨ
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_7, ADC_SAMPLETIME_55POINT5);      //PA7 - S2
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_8, ADC_SAMPLETIME_55POINT5);      //PB0 - S1
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);      //PA0 - I_F1
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_6, ADC_SAMPLETIME_55POINT5);      //PA6 - F9-RDټ

    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);      //PA1 - F28VU
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_9, ADC_SAMPLETIME_55POINT5);      //PB1 - S0
    adc_regular_channel_config(ADC0, 6, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);      //PA5 - ¶/Ԥ


    		
    /* ADC */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE); //
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);                             //ʹ
		
    /* ADCӿ*/
    adc_enable(ADC0);
    delay_ms(1);
    /* ADCУ׼͸λУ׼ */
    adc_calibration_enable(ADC0);

    /* ADC DMAʹe */
    adc_dma_mode_enable(ADC0);

    /* ADC*/
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);


#else
    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1U);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_NONE);
    /* ADC external trigger config */
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);
#endif
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
#ifdef ADC_DMA_SCAN
/*******************************************************************************
** 函数名称: dma_config
** 功能描述: dma_config函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    /* 变量说明：dma_data_parameter，dma_data_parameter变量，用于保存当前模块运行过程中的状态或临时数据。 */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(adc1_buf);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = adc1_order_num*adc1_ch_num;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);
    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);
}
#endif
/*!
    \brief      configure the different system clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
/*******************************************************************************
** 函数名称: rcu_config
** 功能描述: rcu_config函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void rcu_config(void)
{
    /* enable GPIO clock */
    //    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC0);
    /* enable DMA0 clock */
    //    rcu_periph_clock_enable(RCU_DMA0);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV4);
}
/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
// void gpio_config(void)
//{
/* config the GPIO as analog mode */
//    gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_3);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_1);
//    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_4);
//}

//short Temp_transition(uint16_t adc_data)//¶ȼ
//{
//#define Rp 10000      //10k
//#define T2 298.15     //  T2,273.15+25.0
//#define Bx 3435.0    //Bֵ
//#define Ka 273.15
//#define Rup
//#define Ui 3.3
//    short temp = 0;                               //¶ֵ
//    float Uo;                                   //Ĺѹ
//    float rt = 0;                                 //ֵ
//    Uo = adc_data * 3.3 / 4096;                         //ѹĵѹ
//#ifdef Rup
//	    rt=(Rp*Ui)/(Uo)-Rp;                          //λõֵ,

//#else
//			rt = (Rp * Uo) / (Ui - Uo);                          //λõֵ,
//#endif

//    temp = (short)((1 / (logf(rt / Rp) / Bx + (1 / T2))) - Ka + 0.5);     //¶
//    return temp;


//}

//short Temp_transition(uint16_t adc_data)//¶ȼ
//{
//#define Rp 10000      //10k
//#define T2 298.15     //  T2,273.15+25.0
//#define Bx 3435.0    //Bֵ
//#define Ka 273.15
//#define Rup
//#define Ui 3.3
//    short temp = 0;                               //¶ֵ
//    float Uo;                                   //Ĺѹ
//    float rt = 0;                                 //ֵ
//    Uo = adc_data * 3.3 / 4096;                         //ѹĵѹ
//#ifdef Rup
//    rt = (Rp * Uo) / (Ui - Uo);                          //λõֵ,
//#else
//    rt=(Rp*Ui)/(Uo)-Rp;                          //λõֵ,
//#endif

//    temp = (short)((1 / (logf(rt / Rp) / Bx + (1 / T2))) - Ka + 0.5);     //¶
//    return temp;


//}


#ifndef ADC_DMA_SCAN
/*******************************************************************************
** 函数名称: adc_channel_sample
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
uint16_t adc_channel_sample(uint8_t channel)
{
    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0U, channel, ADC_SAMPLETIME_7POINT5);
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

    /* wait the end of conversion flag */
    while (!adc_flag_get(ADC0, ADC_FLAG_EOC))
        ;
    /* clear the end of conversion flag */
    adc_flag_clear(ADC0, ADC_FLAG_EOC);
    /* return regular channel sample value */
    return (adc_regular_data_read(ADC0));
}

/*******************************************************************************
** 函数名称: Get_Adc_Average
** 功能描述: ADC采样、滤波或数据换算函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	/* 变量说明：temp_val，temp_val变量，用于保存当前模块运行过程中的状态或临时数据。 */
	uint32_t temp_val=0;
	/* 变量说明：t，局部临时变量，用于循环、长度、状态或字节处理。 */
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=adc_channel_sample(ch);
	}
	return temp_val/times;
} 
#endif
