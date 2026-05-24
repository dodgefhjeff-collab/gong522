/**
  ************************************* Copyright ****************************** 
  *
  *                          (C) Copyright 2023,,China.
  *                                  All Rights Reserved
  *                              
  *                             By(ܵӿƼ޹˾)
  *                     
  *    
  * ļ	   : DMA.c   
  * 汾       : v1.0		
  *        : 			
  * ʱ       : 2023-01-29         
  *        :    
  * б   :  
  	1. ....
  	      <汾>: 		
      <޸Ա>:
  		  <ʱ>:
          <>:  
  	2. ...
  ******************************************************************************
 */
#include "dma.h"
#include "gd32c10x.h"
#include "usart.h"
/*******************************************************************************
** 函数名称: MYDMA_Config
** 功能描述: DMA初始化或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void MYDMA_Config()
{


    /* 变量说明：dma_init_struct，dma_init_struct变量，用于保存当前模块运行过程中的状态或临时数据。 */
    dma_parameter_struct dma_init_struct;			//ṹ
    /* enable DMA0 */
    rcu_periph_clock_enable(RCU_DMA0);				//ʹDMAʱ
    /* initialize USART */
    // gd_eval_com_init(EVAL_COM0);

//    //0
//    /* deinitialize DMA channel3(USART0 tx) */
//    dma_deinit(DMA0, DMA_CH3);						//λͨ3
//    dma_struct_para_init(&dma_init_struct);			//DMAṹвʼΪĬֵ
//    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;	// DMAͨݴ䷽ ڴ TO 
//    dma_init_struct.memory_addr = (uint32_t)USART_TX_BUF;		//洢ַ
//    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
//    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;	//洢ݴ
//    dma_init_struct.number = USART_TX_LEN;			//DMAͨݴ
//    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);//ַ
//    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
//    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
//    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;		//DMAͨȼ
//    dma_init(DMA0, DMA_CH3, &dma_init_struct);				//ʼDMA
//    /* configure DMA mode */
//		dma_interrupt_enable(DMA0,DMA_CH3,DMA_INT_FTF);         //ж
//    dma_circulation_disable(DMA0, DMA_CH3); 				//DMAѭģʽ
//    /* enable DMA channel3 */
//    //dma_channel_enable(DMA0, DMA_CH3);						//DMAxͨyʹ  ݲ

//    /* USART DMA enable for transmission and reception */
////    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);	// USART DMA͹
////    usart_dma_receive_config(USART0, USART_DENR_ENABLE);	// USART DMAչ

//    /* wait DMA Channel transfer complete */
//    //while(RESET == dma_flag_get(DMA0, DMA_CH3, DMA_INTF_FTFIF)){};    //ȴȫת
//   //0
//    /* deinitialize DMA channel4 (USART0 rx) */
//    dma_deinit(DMA0, DMA_CH4);							//λͨ4
//    dma_struct_para_init(&dma_init_struct);				//DMAṹвʼΪĬֵ
//    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;// DMAͨݴ䷽   TO ڴ
//    dma_init_struct.memory_addr = (uint32_t)USART_RX_BUF;	 //洢ַ
//    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
//    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;//洢ݴ
//    dma_init_struct.number = USART_REC_IDLE_LEN ;//DMAͨݴ
//    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART0);//ַ
//    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
//    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
//    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;//DMAͨȼ
//    dma_init(DMA0, DMA_CH4, &dma_init_struct);//ʼDMA
//    /* configure DMA mode */
//    dma_circulation_disable(DMA0, DMA_CH4); //DMAѭģʽ
//    /* enable DMA channel4 */
//    dma_channel_enable(DMA0, DMA_CH4);//DMAxͨyʹ

//    /* wait DMA channel transfer complete */
//    //while(RESET == dma_flag_get(DMA0, DMA_CH4, DMA_INTF_FTFIF));//ȴȫת
   

   //1
    dma_deinit(DMA0, DMA_CH6);						//λͨ6
    dma_struct_para_init(&dma_init_struct);			//DMAṹвʼΪĬֵ
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;	// DMAͨݴ䷽ ڴ TO 
    dma_init_struct.memory_addr = (uint32_t)USART1_TX_BUF;		//洢ַ
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;	//洢ݴ
    dma_init_struct.number = USART1_TX_LEN;			//DMAͨݴ
   dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);		//ַ
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;		//DMAͨȼ
    dma_init(DMA0, DMA_CH6, &dma_init_struct);				//ʼDMA
    // /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH6); 				//DMAѭģʽ
    dma_interrupt_enable(DMA0,DMA_CH6,DMA_INT_FTF);         //ж


    /* enable DMA channel6 */
    //dma_channel_enable(DMA0, DMA_CH6);						//DMAxͨyʹ   ݲ

    /* USART DMA enable for transmission and reception */
    usart_dma_transmit_config(USART1, USART_DENT_ENABLE);	// USART DMA͹
    usart_dma_receive_config(USART1, USART_DENR_ENABLE);	//USART DMAչ

    /* wait DMA Channel transfer complete */
    //while(RESET == dma_flag_get(DMA0, DMA_CH6, DMA_INTF_FTFIF)){};    //ȴȫת
//1
    /* deinitialize DMA channel5 (USART0 rx) */
    dma_deinit(DMA0, DMA_CH5);							//λͨ5
    dma_struct_para_init(&dma_init_struct);				//DMAṹвʼΪĬֵ
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;// DMAͨݴ䷽   TO ڴ
    dma_init_struct.memory_addr = (uint32_t)USART1_RX_BUF;	 //洢ַ
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;//洢ݴ
    dma_init_struct.number = USART1_REC_IDLE_LEN ;//DMAͨݴ
    dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART1);//ַ
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;//DMAͨȼ
    dma_init(DMA0, DMA_CH5, &dma_init_struct);//ʼDMA
    // /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH5); //DMAѭģʽ
    // /* enable DMA channel5 */
    dma_channel_enable(DMA0, DMA_CH5);//DMAxͨyʹ

    /* wait DMA channel transfer complete */
////    while(RESET == dma_flag_get(DMA0, DMA_CH5, DMA_INTF_FTFIF));//ȴȫת

//2
    // dma_deinit(DMA0, DMA_CH1);						//λͨ1
    // dma_struct_para_init(&dma_init_struct);			//DMAṹвʼΪĬֵ
    // dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;	// DMAͨݴ䷽ ڴ TO 
    // dma_init_struct.memory_addr = (uint32_t)USART2_TX_BUF;		//洢ַ
    // dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    // dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;	//洢ݴ
    // dma_init_struct.number = USART2_TX_LEN;			//DMAͨݴ
    // dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART2);		//ַ
    // dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    // dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
    // dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;		//DMAͨȼ
    // dma_init(DMA0, DMA_CH1, &dma_init_struct);				//ʼDMA
    // /* configure DMA mode */
    // dma_circulation_disable(DMA0, DMA_CH1); 				//DMAѭģʽ
    // dma_interrupt_enable(DMA0,DMA_CH1,DMA_INT_FTF);         //ж
    /* enable DMA channel6 */
    //  dma_channel_enable(DMA0, DMA_CH1);						//DMAxͨyʹ  ݲ

    /* USART DMA enable for transmission and reception */
//    usart_dma_transmit_config(USART2, USART_DENT_ENABLE);	// USART DMA͹
//    usart_dma_receive_config(USART2, USART_DENR_ENABLE);	//USART DMAչ

    /* wait DMA Channel transfer complete */
    //while(RESET == dma_flag_get(DMA0, DMA_CH1, DMA_INTF_FTFIF)){};    //ȴȫת
//2
    // dma_deinit(DMA0, DMA_CH2);							//λͨ2
    // dma_struct_para_init(&dma_init_struct);				//DMAṹвʼΪĬֵ
    // dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;// DMAͨݴ䷽   TO ڴ
    // dma_init_struct.memory_addr = (uint32_t)USART2_RX_BUF;	 //洢ַ
    // dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;//洢ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    // dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;//洢ݴ
    // dma_init_struct.number = USART2_REC_IDLE_LEN ;//DMAͨݴ
    // dma_init_struct.periph_addr = (uint32_t)&USART_DATA(USART2);//ַ
    // dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;//ַ㷨ģʽ  	ENABLE ַ  DISABLE ̶ַ
    // dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;//ݴ
    // dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;//DMAͨȼ
    // dma_init(DMA0, DMA_CH2, &dma_init_struct);//ʼDMA
    // /* configure DMA mode */
    // dma_circulation_disable(DMA0, DMA_CH2); //DMAѭģʽ
    // /* enable DMA channel5 */
    // dma_channel_enable(DMA0, DMA_CH2);//DMAxͨyʹ

    /* wait DMA channel transfer complete */
    //while(RESET == dma_flag_get(DMA0, DMA_CH2, DMA_INTF_FTFIF));//ȴȫת



}






