/*
**----------------------------------ļϢ------------------------------------
** ļ: TIM.c
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

#include "TIM.h"

/*******************************************************************************
** 函数名称: TIM2_init_all
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void TIM2_init_all(u16 arr,u16 psc)
{
    /* 变量说明：TIM_TimeBaseStructure，TIM_TimeBaseStructure变量，用于保存当前模块运行过程中的状态或临时数据。 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱʹ

    //ʱTIM3ʼ
    TIM_TimeBaseStructure.TIM_Period = arr; //һ¼װԶװؼĴڵֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //ΪTIMxʱƵʳԤƵֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱӷָ:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMϼģʽ
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //ָĲʼTIMxʱλ
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹָTIM3ж,ж
    TIM_Cmd(TIM2, ENABLE);  //ʹTIMx


}


/*******************************************************************************
** 函数名称: TIM3_init_all
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void TIM3_init_all(u16 arr,u16 psc)
{
    /* 变量说明：TIM_TimeBaseStructure，TIM_TimeBaseStructure变量，用于保存当前模块运行过程中的状态或临时数据。 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱʹ

    //ʱTIM3ʼ
    TIM_TimeBaseStructure.TIM_Period = arr; //һ¼װԶװؼĴڵֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //ΪTIMxʱƵʳԤƵֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱӷָ:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMϼģʽ
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //ָĲʼTIMxʱλ
    //TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹָTIM3ж,ж

#ifdef TimerTriggerADC

    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
    //TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE ); //ʹָTIM3жϣж
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update); //жһжϺж
    //TIM_Cmd(TIM3, ENABLE);  //ʹTIM1
#endif

    TIM_Cmd(TIM3, ENABLE);  //ʹTIMx
}

/*******************************************************************************
** 函数名称: TIM4_init_all
** 功能描述: 定时器或PWM配置/处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void TIM4_init_all(u16 arr,u16 psc)
{
    /* 变量说明：TIM_TimeBaseStructure，TIM_TimeBaseStructure变量，用于保存当前模块运行过程中的状态或临时数据。 */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱʹ
    //ʱTIM4ʼ
    TIM_TimeBaseStructure.TIM_Period = arr; //һ¼װԶװؼĴڵֵ
    TIM_TimeBaseStructure.TIM_Prescaler =psc; //ΪTIMxʱƵʳԤƵֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //ʱӷָ:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIMϼģʽ
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //ָĲʼTIMxʱλ
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹָTIM4ж,ж
    TIM_Cmd(TIM4, ENABLE);  //ʱʹTIMx
}

