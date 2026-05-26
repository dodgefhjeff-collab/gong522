/* 文件：pwm.c，GD32C103CBT6 PWM输出初始化 */

#include "pwm.h"

/* TIM1_PWM_Init：定时器或PWM配置/处理函数 */
void TIM1_PWM_Init(uint16_t arr, uint16_t psc)
{
    
    timer_parameter_struct timer_initpara;
    
    timer_oc_parameter_struct timer_ocintpara;

    rcu_periph_clock_enable(RCU_TIMER0);        // 原TIM1 -> GD32 TIMER0

    timer_deinit(TIMER0);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = psc;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 50);
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_ENABLE);

    timer_auto_reload_shadow_enable(TIMER0);
    timer_primary_output_config(TIMER0, ENABLE);
    timer_enable(TIMER0);
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 499);
}

/* TIM4_PWM_Init：定时器或PWM配置/处理函数 */
void TIM4_PWM_Init(uint16_t arr, uint16_t psc)
{
    
    timer_parameter_struct timer_initpara;
    
    timer_oc_parameter_struct timer_ocintpara;

    rcu_periph_clock_enable(RCU_TIMER3);        // 原TIM4 -> GD32 TIMER3

    timer_deinit(TIMER3);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = psc;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER3, &timer_initpara);

    timer_channel_output_struct_para_init(&timer_ocintpara);
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_LOW;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;

    timer_channel_output_config(TIMER3, TIMER_CH_2, &timer_ocintpara);
    timer_channel_output_config(TIMER3, TIMER_CH_3, &timer_ocintpara);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 50);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 50);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_2, TIMER_OC_MODE_PWM1);
    timer_channel_output_mode_config(TIMER3, TIMER_CH_3, TIMER_OC_MODE_PWM1);
    timer_channel_output_shadow_config(TIMER3, TIMER_CH_2, TIMER_OC_SHADOW_ENABLE);
    timer_channel_output_shadow_config(TIMER3, TIMER_CH_3, TIMER_OC_SHADOW_ENABLE);

    timer_auto_reload_shadow_enable(TIMER3);
    timer_enable(TIMER3);

    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_2, 20);
    timer_channel_output_pulse_value_config(TIMER3, TIMER_CH_3, 20);
}
