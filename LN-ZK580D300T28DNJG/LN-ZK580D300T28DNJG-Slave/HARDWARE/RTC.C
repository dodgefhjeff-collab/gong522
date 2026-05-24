/**
  ************************************* Copyright ******************************
  *
  *                          (C) Copyright 2022,胡兴明,China.
  *                                  All Rights Reserved
  *
  *                             By(陕西雷能电子科技有限公司)
  *
  *
  * 文件名	   : RTC.C
  * 版本       : v1.0
  * 作者       : 胡兴明
  * 时间       : 2022-08-15
  * 描述       : 移植正点原子RTC相关代码
  * 函数列表   :
  	1. ....
  	      <版本>:
      <修改人员>:
  		  <时间>:
          <描述>:
  	2. ...
  ******************************************************************************
**/
#include<stdio.h>
#include<stdlib.h>
#include "RTC.h"
#include "delay.h"
#include "adc.h"
extern uint32_t CumulWorkTime;     //累计工作时间  Cumulative working time
extern uint32_t DurationOfWork;    //本次工作时长 Duration of work
extern uint8_t Alarm;              // 1分钟定时
_calendar_obj calendar;//时钟结构体
_calendar_obj Fault_RQ;
//uint8_t RTC_Init(void)
//{
//	//检查是不是第一次配置时钟
//	uint8_t temp=0;
//    rcu_periph_clock_enable(RCU_RTC);                       //使能RTC时钟
//    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);                 //选择时钟源
//    rcu_osci_stab_wait(RCU_LXTAL);                          //等待时钟稳定


//}
void rtc_configuration(void)
{
    /* enable PMU and BKPI clocks 使能电源时钟和备份区域时钟*/
    rcu_periph_clock_enable(RCU_BKPI);
    rcu_periph_clock_enable(RCU_PMU);

    /* allow access to BKP domain 允许访问BKP区域*/
    pmu_backup_write_enable();

    //复位备份区域，开启外部低速振荡器
    /* reset backup domain */
    bkp_deinit();

    /* enable LXTAL 使能外部低速晶振 32.768K */
    rcu_osci_on(RCU_LXTAL);
    /* wait till LXTAL is ready */
    rcu_osci_stab_wait(RCU_LXTAL);

    //选择 RTC 时钟，并使能
    /* select RCU_LXTAL as RTC clock source */
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    /* enable RTC Clock 使能RTC时钟	 */
    rcu_periph_clock_enable(RCU_RTC);

    rtc_configuration_mode_enter();

    /* wait for RTC registers synchronization */
    rtc_register_sync_wait();

    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    /* enable the RTC second interrupt 使能RTC秒中断、闹钟*/
		
    //rtc_interrupt_enable(RTC_INT_SECOND);
    //rtc_interrupt_enable(RTC_INT_ALARM);
		
    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    /* set RTC prescaler: set RTC period to 1s 设置预分频*/
    rtc_prescaler_set(32767);

    /* wait until last write operation on RTC registers has finished 等待写RTC寄存器完成*/
    rtc_lwoff_wait();

    RTC_Set(2022,10,6,18,58,00);
    rtc_alarm_config (rtc_counter_get()+60);

}


//RTC时钟中断
//每秒触发一次
//extern uint16_t tcnt;
void RTC_IRQHandler(void)
{
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET)//秒钟中断
    {
        rtc_flag_clear(RTC_FLAG_SECOND);
        	RTC_Get();//更新时间
//        printf("temp: %d \n",Get_the_temperature_value());
        //RunningTime++;
        //printf("\r\n %d - %d - %d %d:%d:%d  %d\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec,calendar.week);
    }
    if(rtc_flag_get(RTC_FLAG_ALARM)!= RESET)//闹钟中断
    {
        rtc_flag_clear(RTC_FLAG_ALARM);

//        rtc_alarm_config (rtc_counter_get()+60);
//        CumulWorkTime+=60;     //累计工作时间
//        DurationOfWork+=60;    //本次工作时长
//        Alarm=1;               //标记定时时间到
        //RTC_Get();				//更新时间
       // printf("时间到了");
//        printf("Alarm Time:%d-%d-%d %d:%d:%d\n",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间

    }
    rtc_flag_clear(RTC_FLAG_OVERFLOW);	//溢出中断
    rtc_lwoff_wait();                   //等待最近一次对RTC寄存器的写操作完成
}
//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
uint8_t Is_Leap_Year(uint16_t year)
{
    if(year%4==0) //必须能被4整除
    {
        if(year%100==0)
        {
            if(year%400==0)return 1;//如果以00结尾,还要能被400整除
            else return 0;
        } else return 1;
    } else return 0;
}
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//返回值:0,成功;其他:错误代码.
//月份数据表
uint8_t const table_week[12]= {0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表
//平年的月份日期表
const uint8_t mon_table[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
uint8_t RTC_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
    uint16_t t;
    uint32_t seccount=0;
    if(syear<2022||syear>2099)return 1;
    for(t=2022; t<syear; t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
        else seccount+=31536000;			  //平年的秒钟数
    }
    smon-=1;
    for(t=0; t<smon; t++)	 //把前面月份的秒钟数相加
    {
        seccount+=(uint32_t)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
    }
    seccount+=(uint32_t)(sday-1)*86400;//把前面日期的秒钟数相加
    seccount+=(uint32_t)hour*3600;//小时秒钟数
    seccount+=(uint32_t)min*60;	 //分钟秒钟数
    seccount+=sec;//最后的秒钟加上去

    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR , ENABLE);	//使能PWR和BKP外设时钟
    //PWR_BackupAccessCmd(ENABLE);	//使能RTC和后备寄存器访问
    rtc_counter_set (seccount);	//设置RTC计数器的值
    rtc_lwoff_wait();	//等待最近一次对RTC寄存器的写操作完成
    return 0;
}

//初始化闹钟
//以1970年1月1日为基准
//1970~2099年为合法年份
//syear,smon,sday,hour,min,sec：闹钟的年月日时分秒
//返回值:0,成功;其他:错误代码.
uint8_t RTC_Alarm_Set(uint16_t syear,uint8_t smon,uint8_t sday,uint8_t hour,uint8_t min,uint8_t sec)
{
    uint16_t t;
    uint32_t seccount=0;
    if(syear<2022||syear>2099)return 1;
    for(t=2022; t<syear; t++)	//把所有年份的秒钟相加
    {
        if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
        else seccount+=31536000;			  //平年的秒钟数
    }
    smon-=1;
    for(t=0; t<smon; t++)	 //把前面月份的秒钟数相加
    {
        seccount+=(uint32_t)mon_table[t]*86400;//月份秒钟数相加
        if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//闰年2月份增加一天的秒钟数
    }
    seccount+=(uint32_t)(sday-1)*86400;//把前面日期的秒钟数相加
    seccount+=(uint32_t)hour*3600;//小时秒钟数
    seccount+=(uint32_t)min*60;	 //分钟秒钟数
    seccount+=sec;//最后的秒钟加上去
    //设置时钟
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR和BKP外设时钟
    //PWR_BackupAccessCmd(ENABLE);	//使能后备寄存器访问
    //上面三步是必须的!

    rtc_alarm_config (seccount);

    rtc_lwoff_wait();	//等待最近一次对RTC寄存器的写操作完成

    return 0;
}


//得到当前的时间
//返回值:0,成功;其他:错误代码.
uint8_t RTC_Get(void)
{
    static uint16_t daycnt=0;
    uint32_t timecount=0;
    uint32_t temp=0;
    uint16_t temp1=0;
    timecount=rtc_counter_get();
    temp=timecount/86400;   //得到天数(秒钟数对应的)
    if(daycnt!=temp)//超过一天了
    {
        daycnt=temp;
        temp1=2022;	//从2022年开始
        while(temp>=365)
        {
            if(Is_Leap_Year(temp1))//是闰年
            {
                if(temp>=366)temp-=366;//闰年的秒钟数
                else {
                    temp1++;
                    break;
                }
            }
            else temp-=365;	  //平年
            temp1++;
        }
        calendar.w_year=temp1;//得到年份
        temp1=0;
        while(temp>=28)//超过了一个月
        {
            if(Is_Leap_Year(calendar.w_year)&&temp1==1)//当年是不是闰年/2月份
            {
                if(temp>=29)temp-=29;//闰年的秒钟数
                else break;
            }
            else
            {
                if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
                else break;
            }
            temp1++;
        }
        calendar.w_month=temp1+1;	//得到月份
        calendar.w_date=temp+1;  	//得到日期
    }
    temp=timecount%86400;     		//得到秒钟数
    calendar.hour=temp/3600;     	//小时
    calendar.min=(temp%3600)/60; 	//分钟
    calendar.sec=(temp%3600)%60; 	//秒钟
    calendar.week=RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);//获取星期
    return 0;
}
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
uint8_t RTC_Get_Week(uint16_t year,uint8_t month,uint8_t day)
{
    uint16_t temp2;
    uint8_t yearH,yearL;

    yearH=year/100;
    yearL=year%100;
    // 如果为21世纪,年份数加100
    if (yearH>19)yearL+=100;
    // 所过闰年数只算1900年之后的
    temp2=yearL+yearL/4;
    temp2=temp2%7;
    temp2=temp2+day+table_week[month-1];
    if (yearL%4==0&&month<3)temp2--;
    return(temp2%7);
}

