/* 文件：tim.c，GD32C103CBT6 TIMER初始化，保留原TIM2/TIM3/TIM4接口名 */

#include "tim.h"
#include "power.h"
#include "FreeRTOSConfig.h"

#if configGENERATE_RUN_TIME_STATS

volatile unsigned long long FreeRTOSRunTimeTicks = 0;
#endif

static void Timer_Base_Config(uint32_t timer_periph, uint16_t arr, uint16_t psc, uint8_t enable)
{
    
    timer_parameter_struct timer_initpara;

    timer_deinit(timer_periph);
    timer_struct_para_init(&timer_initpara);
    timer_initpara.prescaler         = psc;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(timer_periph, &timer_initpara);

    timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(timer_periph, TIMER_INT_UP);

    if (enable)
    {
        timer_enable(timer_periph);
    }
    else
    {
        timer_disable(timer_periph);
    }
}

#if configGENERATE_RUN_TIME_STATS

void ConfigureTimeForRunTimeStats(void)
{
    rcu_periph_clock_enable(RCU_TIMER3);
    Timer_Base_Config(TIMER3, 99, 71, 1);       // 原TIM4，运行时间统计
}
#else
/* TIM4_init_all：定时器或PWM配置/处理函数 */
void TIM4_init_all(uint16_t arr, uint16_t psc)
{
    rcu_periph_clock_enable(RCU_TIMER3);
    Timer_Base_Config(TIMER3, arr, psc, 1);     // 原TIM4
}
#endif

/* TIM2_init_all：定时器或PWM配置/处理函数 */
void TIM2_init_all(uint16_t arr, uint16_t psc)
{
    rcu_periph_clock_enable(RCU_TIMER1);
    Timer_Base_Config(TIMER1, arr, psc, 1);     // 原TIM2
}

/* TIM3_init_all：定时器或PWM配置/处理函数 */
void TIM3_init_all(uint16_t arr, uint16_t psc)
{
    rcu_periph_clock_enable(RCU_TIMER2);
    Timer_Base_Config(TIMER2, arr, psc, 0);     // 原TIM3，初始化后暂不启动
}
