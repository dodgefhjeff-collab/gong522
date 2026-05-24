/**
  ************************************* Copyright ******************************
  *  
  *                         (C) Copyright 2023,胡兴明,China.
  *                                 All Rights Reserved
  *                              
  *                            By(陕西雷能电子科技有限公司)
  *                     
  *      
  * 文件名       : timer.h   
  * 版本         : v1.0		
  * 作者         : 胡兴明			
  * 时间         : 2023-01-29         
  * 描述         :    
  * 函数列表     :  
  	1. ....
  	      <版本>: 		
      <修改人员>:
  		  <时间>:
          <描述>:  
  	2. ...
  ******************************************************************************
 */

#ifndef  TIMER_H
/* 宏定义说明：TIMER_H，TIMER_H宏定义，用于保持原工程风格并集中配置相关参数。 */
#define  TIMER_H
#include "freeRTOSConfig.h"
/* 函数声明说明：timer1_config，定时器或PWM配置/处理函数。 */
void timer1_config(void);
/* 函数声明说明：timer2_config，定时器或PWM配置/处理函数。 */
void timer2_config(void);
#if configGENERATE_RUN_TIME_STATS 
/* 函数声明说明：ConfigureTimeForRunTimeStats，ConfigureTimeForRunTimeStats函数，保持原工程接口并完成对应模块处理。 */
void ConfigureTimeForRunTimeStats(void);
#endif
#endif /* TIMER_H */

