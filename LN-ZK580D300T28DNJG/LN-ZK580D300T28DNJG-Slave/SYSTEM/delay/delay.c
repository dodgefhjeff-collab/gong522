/*
**----------------------------------文件信息------------------------------------
** 文件名称: delay.c
** 创建人员: 正点原子
** 创建日期: 
** 文档描述:
**
**----------------------------------版本信息------------------------------------
** 版本代号: V0.1
** 版本说明: 初始版本
**
**------------------------------------------------------------------------------
*/

#include "delay.h"
#include "gd32c10x.h"
#include "gd32c10x_misc.h"
static uint8_t  fac_us=0;							//us延时倍乘数			   

void delay_Init(void)
{
	//SysTick时钟源选择
	systick_clksource_set(SYSTICK_CLKSOURCE_HCLK);
	//延时1us需要SysTick个时钟节拍
	fac_us = SystemCoreClock/1000000;
	//重装值
	SysTick->LOAD = 0xFFF;
	//开启计数
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void delay_us(uint32_t nus)
{
	uint32_t old_cnt,now_cnt,cnt = 0;
	//重装值
	uint32_t reload = SysTick->LOAD;
	//延时节拍
	uint32_t ticks = nus * fac_us;
	//记录节拍
	old_cnt = SysTick->VAL;
	
	while(1)
	{
		now_cnt = SysTick->VAL;
		if(old_cnt != now_cnt)
		{
			if(now_cnt < old_cnt)
				cnt += (old_cnt - now_cnt);
			else
				cnt += (reload + old_cnt - now_cnt);
			
			old_cnt = now_cnt;
			if(cnt >= ticks)
				break;
		}
	}
}

void delay_ms(uint16_t nms)
{
	uint32_t i;
	
	for(i = 0;i < nms;i++)
	{
		delay_us(1000);
	}
}


