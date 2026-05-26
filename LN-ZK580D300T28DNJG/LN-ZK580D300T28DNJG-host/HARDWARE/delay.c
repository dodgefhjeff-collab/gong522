#include "gd32c10x.h"
#include "delay.h"

/*
 * GD32C103 兼容旧 STM32 工程的阻塞延时接口。
 * 这里只用于启动阶段和少量短延时；FreeRTOS 调度启动后，任务里优先使用 vTaskDelay()。
 */
void delay_init(void)
{
    /* 保留旧工程接口。GD32 SystemCoreClock 已由 system_gd32c10x.c 维护。 */
    SystemCoreClockUpdate();
}

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    if (nus == 0U)
    {
        return;
    }

    reload = SysTick->LOAD;
    ticks = nus * (SystemCoreClock / 1000000U);
    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

void delay_ms(uint32_t nms)
{
    while (nms--)
    {
        delay_us(1000U);
    }
}
