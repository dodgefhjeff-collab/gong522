/*
**----------------------------------ļϢ------------------------------------
** ļ: PWM.c
** Ա: 
** : 2021-3-23
** ĵ:
**
**----------------------------------汾Ϣ------------------------------------
** 汾: V0.1
** 汾˵: ʼ汾
**
**------------------------------------------------------------------------------
*/
#include "PWM.H"

/* 变量说明：F_PWM_Value，风机PWM当前比较值缓存，用于28PIN上传给48PIN。 */
uint16_t F_PWM_Value = 0;

//TIM1 CH1 PWM
//PWMʼ
//arrԶװֵ
//pscʱԤƵ
/*******************************************************************************
** 函数名称: pwm_config
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void pwm_config(uint16_t arr,uint16_t psc)
{
    /* -----------------------------------------------------------------------
    TIMER1 configuration: generate 3 PWM signals with 3 different duty cycles:
    TIMER1CLK = SystemCoreClock / 120 = 1MHz, the PWM frequency is 62.5Hz.

    TIMER1 channel0 duty cycle = (4000/ 16000)* 100  = 25%
    TIMER1 channel1 duty cycle = (8000/ 16000)* 100  = 50%
    TIMER1 channel2 duty cycle = (12000/ 16000)* 100 = 75%
    ----------------------------------------------------------------------- */
    /* 变量说明：timer_ocinitpara，timer_ocinitpara变量，用于保存当前模块运行过程中的状态或临时数据。 */
    timer_oc_parameter_struct timer_ocinitpara;						//ṹ
    /* 变量说明：timer_initpara，timer_initpara变量，用于保存当前模块运行过程中的状态或临时数据。 */
    timer_parameter_struct timer_initpara;								//ṹ

    rcu_periph_clock_enable(RCU_TIMER0);									//ʹʱ

    timer_deinit(TIMER0);																	//λʱ1
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);							//ݳʼ
    /* TIMER0 configuration */
    timer_initpara.prescaler         = psc;								//Ƶϵ
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;//ģʽ
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;	//
    timer_initpara.period            = arr;						 	  //
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;	//ʱӷƵ
    timer_initpara.repetitioncounter = 0;									//ظֵ
    timer_init(TIMER0, &timer_initpara);									//ʼʱ1

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);//TIMERͨݽṹвʼΪĬֵ
    /* CH0, CH1 and CH2 configuration in PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;					//ͨ״̬
    timer_ocinitpara.outputnstate = TIMER_CCXN_DISABLE;				//ͨ״̬
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;		//ͨ
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;	//ͨ
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;	//״̬
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;	//״̬»

    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);	//ͨͨ

    /* CH0 configuration in PWM mode0 */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 1);	//Ƚֵ
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM1);//Ƚģʽ
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);//ͨӰ

    timer_primary_output_config(TIMER0, ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);//ԶװӰʹ
    /* auto-reload preload enable */
    timer_enable(TIMER0);										//ʱʹ

    /* 上电默认低比较值，反相PWM下对应较高转速，与断开PWM线全速行为一致 */
    F_PWM_SetValue(0);
}

/*******************************************************************************
** 函数名称: F_PWM_SetValue
** 功能描述: F_PWM_SetValue函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void F_PWM_SetValue(uint16_t value)
{
    F_PWM_Value = value;
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, value);
}

