/**
  ************************************* Copyright ****************************** 
  *
  *                          (C) Copyright 2023,胡兴明,China.
  *                                  All Rights Reserved
  *                              
  *                             By(陕西雷能电子科技有限公司)
  *                     
  *    
  * 文件名	   : CAN.c   
  * 版本       : v1.0		
  * 作者       : 胡兴明			
  * 时间       : 2023-01-29         
  * 描述       :    
  * 函数列表   :  
  	1. ....
  	      <版本>: 		
      <修改人员>:
  		  <时间>:
          <描述>:  
  	2. ...
  ******************************************************************************
 */
#include "power.h"
		
can_receive_message_struct g_receive_message;
can_filter_parameter_struct can_filter_parameter_entity;
FlagStatus can0_receive_flag;
FlagStatus can1_receive_flag;
/*!
    \brief      configure GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);			//使能CAN0时钟
    rcu_periph_clock_enable(RCU_GPIOA);			//使能GPIOA时钟
    rcu_periph_clock_enable(RCU_AF);				//使能复用
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    

	
}

void can_config(void)
{
    can_parameter_struct can_parameter;
     
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);//初始化所用结构体		
    /* initialize CAN register */
    can_deinit(CAN0);							//复位CAN0
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;				//时间触发通讯模式
		can_parameter.resync_jump_width = 2;						//再同步补偿宽度
		can_parameter.time_segment_1 = CAN_BT_BS1_11TQ;	//位段1
		can_parameter.time_segment_2 = CAN_BT_BS2_3TQ;	//位段2
    can_parameter.auto_bus_off_recovery = ENABLE;//自动离线恢复
    can_parameter.auto_wake_up = ENABLE;					//自动唤醒
    can_parameter.auto_retrans = ENABLE;					//自动重传
    can_parameter.rec_fifo_overwrite = DISABLE;		//接收FIFO满时覆盖
    can_parameter.trans_fifo_order = DISABLE;			//发送FIFO顺序
    can_parameter.working_mode = CAN_NORMAL_MODE; //工作模式 
		can_parameter.prescaler=5;


    /* initialize CAN */
    can_init(CAN0, &can_parameter);								//CAN初始化
    
    /* config CAN0 baud rate */
    //can_frequency_set(CAN0, DEV_CAN_BAUD_RATE);		//通讯波特率配置

    
    /* initialize filter */ 
    can1_filter_start_bank(14);										//CAN1过滤器起始编号
    can_filter_mask_mode_init((Addre<<13), 0x001FE000,CAN_EXTENDED_FIFO0, 0);		//只接收目标地址为addre的ID号数据
		can_filter_mask_mode_init((0XFF<<13), 0x001FE000,CAN_EXTENDED_FIFO1, 1);		//只接收目标地址为FF的ID号数据(FF 是控制所有模块的地址)

    
    /* configure CAN0 NVIC */
    nvic_irq_enable(CAN0_RX0_IRQn, 0, 0);							//中断分组
    nvic_irq_enable(CAN0_RX1_IRQn, 0, 1);							//中断分组
    /* enable can receive FIFO0 not empty interrupt */
    can_interrupt_enable(CAN0, CAN_INT_RFNE0);		//CAN中断使能
    can_interrupt_enable(CAN0, CAN_INT_RFNE1);		//CAN中断使能
	 
}

void canInit()
{
	can_gpio_config();
  can_config();
	
	

  // CAN0_TX_IRQHandler ; 
  // CAN0_RX0_IRQHandler; 
  // CAN0_RX1_IRQHandler; 
  // CAN0_EWMC_IRQHandler;
}

void CAN0_RX0_IRQHandler()
{
	can_interrupt_flag_clear(CAN0,CAN_INT_FLAG_RFF0);//清标记
	can_message_receive(CAN0, CAN_FIFO0, &RxMessage);//读出数据
	//Usart_SendArray(USART0,RxMessage.rx_data,8);
	LoseContact=0;				//清失联
	LED_RUN_fre=1;				//设置闪烁200ms 一次
	CanResFlag=1;
	SouAddre=(RxMessage.rx_efid>>21)&0xFF;		//源地址
	DesAddre=(RxMessage.rx_efid>>13)&0xFF;		//目标地址
	ConCom = (RxMessage.rx_efid>>9)&0x0F;		//控制命令
	//mult=RxMessage.rx_efid & 0x1FF;					//多包
//	printf("SouAddre : %d \n",SouAddre);
//	printf("DesAddre : %d \n",DesAddre);
//	printf("ConCom : %d \n",ConCom);
	if(SouAddre==0x01 || SouAddre==0x02 || SouAddre==0x03)			//01=广播指令；02=上位机；03=主控
	{
				
		switch (ConCom)			//控制命令
		{
			case 0x01:		//开关控制 需返回数据
				if(RxMessage.rx_data[0]==0x12)				//固定标识0x12
				{
					
					if(RxMessage.rx_data[1]==0x11)			//开
					{
						INH_ZK_EN=ON;
					}
					else if(RxMessage.rx_data[1]==0x22)//关
					{
						INH_ZK_EN=OFF;
					}
					
					if(RxMessage.rx_data[2]==0x11)			//开
					{
						INH_HK_EN=ON;
					}
					else if(RxMessage.rx_data[2]==0x22)//关
					{
						INH_HK_EN=OFF;
					}
					
					if(RxMessage.rx_data[3]==0x11)			//开
					{
						INH_FAN_ON;
					}
					else if(RxMessage.rx_data[3]==0x22)//关
					{
						INH_FAN_OFF;
					}					
				}
				
					if(TX_BKdata1_FLAG==1)													//检查数据1是否准备好
					{
						CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X01,8,CAN_TX_BKdata1);//数据1准备好，发送数据1
						CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X02,7,&CAN_TX_BKdata1[8]);//数据1准备好，发送数据1
					}
					else 
					{
						CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X01,8,CAN_TX_BKdata2);//发送数据2
						CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X02,7,&CAN_TX_BKdata2[8]);//发送数据2
					}
					MReadsOnce=0;		//标识故障信息主控已经读走
				break;
			case 0x02:		//开关控制 无需返回数据
				if(RxMessage.rx_data[0]==0x12)				//固定标识0x12
				{
					if(RxMessage.rx_data[1]==0x11)			//开
					{
						INH_ZK_EN=ON;
					}
					else if(RxMessage.rx_data[1]==0x22)//关
					{
						INH_ZK_EN=OFF;
					}
					
					if(RxMessage.rx_data[2]==0x11)			//开
					{
						INH_HK_EN=ON;
					}
					else if(RxMessage.rx_data[2]==0x22)//关
					{
						INH_HK_EN=OFF;
					}
					
					if(RxMessage.rx_data[3]==0x11)			//开
					{
						INH_FAN_ON;
					}
					else if(RxMessage.rx_data[3]==0x22)//关
					{
						INH_FAN_OFF;
					}
				}
				break;
			case 0x04:		//状态查询
					if(RxMessage.rx_data[0]==0x44)										//0x44固定标识
					{
						if(TX_BKdata1_FLAG==1)													//检查数据1是否准备好
						{
							CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X01,8,CAN_TX_BKdata1);//数据1准备好，发送数据1
							CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X02,7,&CAN_TX_BKdata1[8]);//数据1准备好，发送数据1
						}
						else 
						{
							CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X01,8,CAN_TX_BKdata2);//发送数据2
							CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x06<<9)|0X02,7,&CAN_TX_BKdata2[8]);//发送数据2
						}
					}
					MReadsOnce=0;		//标识故障信息主控已经读走
				break;
			case 0x05:		//版本号查询
				//						(Addre<<21)|(0x03<<13)|(0x07<<9) | 0
				//							源地址     目标地址  控制命令    多包
				CAN0_Trasnmit((Addre<<21)|(0x03<<13)|(0x07<<9) ,5,Version);		//发送版本号
				break;
		}
	}
}

void CAN0_RX1_IRQHandler()
{
	can_interrupt_flag_clear(CAN0,CAN_INT_FLAG_RFF1);//清标记
  
	can_message_receive(CAN0, CAN_FIFO1, &RxMessage);//读出数据

	SouAddre=(RxMessage.rx_efid>>21)&0xFF;		//源地址
	DesAddre=(RxMessage.rx_efid>>13)&0xFF;		//目标地址
	ConCom = (RxMessage.rx_efid>>9)&0x0F;		//控制命令
	
	CanResFlag1=1;
	if(SouAddre==0x03 && DesAddre==0xFF && ConCom==0X03)		//源地址=0x03(主控),目标地址=0XFF (所有板卡),控制命令=0x03（紧急停止）
	{
		if(RxMessage.rx_data[0]==0x33 && RxMessage.rx_data[1]==0x12 && RxMessage.rx_data[2]==0x21)
		{
//			if(RxMessage.rx_data[3]==0x22)	//判断综控需不需要关闭
//			{
//				INH_ZK_OFF;
//			}
			if(RxMessage.rx_data[4]==0x22)			//紧急停机 
			{
				INH_HK_OFF;												//关闭开关
				INH_FAN_OFF;
			}
		}
	}
}

