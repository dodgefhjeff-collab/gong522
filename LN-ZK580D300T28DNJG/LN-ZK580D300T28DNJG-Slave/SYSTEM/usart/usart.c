/*
**----------------------------------ļϢ------------------------------------
** ļ: usart.c
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


#include "usart.h"	

uint8_t USART_RX_STA=0;
uint8_t USART1_RX_STA=0;
uint8_t USART2_RX_STA=0;

uint8_t  USART_RX_BUF[USART_REC_IDLE_LEN];
uint8_t  USART_TX_BUF[USART_TX_LEN];

 uint8_t  USART1_RX_BUF[USART1_REC_IDLE_LEN];
 uint8_t  USART1_TX_BUF[USART1_TX_LEN];

static volatile uint16_t uart1_rx_head = 0;
static volatile uint16_t uart1_rx_tail = 0;

// uint8_t  USART2_RX_BUF[USART2_REC_IDLE_LEN];
// uint8_t  USART2_TX_BUF[USART2_TX_LEN];
//void uart_init(uint32_t bound)
//{
//    rcu_periph_clock_enable(RCU_USART0);
//    usart_deinit(USART0);                               //λUSART0
//    usart_baudrate_set(USART0, bound);                  //ò
//    usart_receive_config(USART0, USART_RECEIVE_ENABLE); //ʹܽ
//    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);//ʹܷ

//    usart_interrupt_enable(USART0,USART_INT_IDLE);      //ʹܿж
//    usart_enable(USART0);                                   //usart0 ʹ 
//    
//    usart_dma_transmit_config(USART0, USART_DENT_ENABLE);	// USART DMA͹
//    usart_dma_receive_config(USART0, USART_DENR_ENABLE);	//USART DMAչ
//}

 /*******************************************************************************
 ** 函数名称: uart1_init
 ** 功能描述: 串口初始化、收发或中断处理函数。
 ** 参数说明: 参见函数形参定义，保持原工程接口不改变。
 ** 返回说明: 参见函数返回类型。
 ********************************************************************************/
 void uart1_init(uint32_t bound)
{
     rcu_periph_clock_enable(RCU_USART1);
     usart_deinit(USART1);                               //λUSART1
     usart_baudrate_set(USART1, bound);                  //ò
     usart_receive_config(USART1, USART_RECEIVE_ENABLE); //ʹܽ
     usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);//ʹܷ

     uart1_clear_rx_buff();
     usart_interrupt_disable(USART1,USART_INT_IDLE);      //ʹܿж
     usart_enable(USART1);                                   //usart1 ʹ 
    
     usart_dma_transmit_config(USART1, USART_DENT_ENABLE);	// USART DMA͹
     usart_dma_receive_config(USART1, USART_DENR_DISABLE);
     usart_interrupt_enable(USART1, USART_INT_RBNE);	//USART DMAչ

 }
// void uart2_init(uint32_t bound){
//     rcu_periph_clock_enable(RCU_USART2);
//     usart_deinit(USART2);                               //λUSART2
//     usart_baudrate_set(USART2, bound);                  //ò
//     usart_receive_config(USART2, USART_RECEIVE_ENABLE); //ʹܽ
//     usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);//ʹܷ

//     usart_interrupt_enable(USART2,USART_INT_IDLE);      //ʹܿж
//     usart_enable(USART2);                                   //usart1 ʹ 
    
//     usart_dma_transmit_config(USART2, USART_DENT_ENABLE);	// USART DMA͹
//     usart_dma_receive_config(USART2, USART_DENR_ENABLE);	//USART DMAչ

// }



/*******************************************************************************
** 函数名称: uart1_rx_byte_process
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void uart1_rx_byte_process(uint8_t ch)
{
    uint16_t next_head;

    next_head = uart1_rx_head + 1;
    if (next_head >= USART1_REC_IDLE_LEN)
    {
        next_head = 0;
    }

    if (next_head != uart1_rx_tail)
    {
        USART1_RX_BUF[uart1_rx_head] = ch;
        uart1_rx_head = next_head;
    }
    else
    {
        USART1_RX_STA = 1;
    }
}


/*******************************************************************************
** 函数名称: uart1_get_rx_byte
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
uint8_t uart1_get_rx_byte(uint8_t *ch)
{
    if (ch == 0)
    {
        return 0;
    }

    if (uart1_rx_head == uart1_rx_tail)
    {
        return 0;
    }

    __disable_irq();
    if (uart1_rx_head != uart1_rx_tail)
    {
        *ch = USART1_RX_BUF[uart1_rx_tail];
        uart1_rx_tail++;
        if (uart1_rx_tail >= USART1_REC_IDLE_LEN)
        {
            uart1_rx_tail = 0;
        }
        __enable_irq();
        return 1;
    }
    __enable_irq();

    return 0;
}

/*******************************************************************************
** 函数名称: uart1_clear_rx_buff
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void uart1_clear_rx_buff(void)
{
    __disable_irq();
    uart1_rx_head = 0;
    uart1_rx_tail = 0;
    USART1_RX_STA = 0;
    __enable_irq();
}

/*******************************************************************************
** 函数名称: fputc
** 功能描述: fputc函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
int fputc(int ch, FILE *f)
{
    /* һֽݵ */
    Usart_SendByte(DEBUG_USARTx, (uint8_t)ch);

    return (ch);
}




/*******************************************************************************
** 函数名称: Usart_SendByte
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Usart_SendByte( uint32_t USARTx, uint8_t ch)
{
	/* һֽݵUSART */
	usart_data_transmit(USARTx,ch);
		
	/* ȴݼĴΪ */
    while (usart_flag_get(USARTx,USART_FLAG_TC)==RESET);
}

/*******************************************************************************
** 函数名称: Usart_SendArray
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Usart_SendArray( uint32_t USARTx, uint8_t *array, uint16_t num)
{
  /* 变量说明：i，局部临时变量，用于循环、长度、状态或字节处理。 */
  uint16_t i;
	
	for(i=0; i<num; i++)
  {
	    /* һֽݵUSART */
	    Usart_SendByte(USARTx,array[i]);	
  
  }
	/* ȴ */
	 while (usart_flag_get(USARTx,USART_FLAG_TC)==RESET);
}

/*****************  ַ **********************/
/*******************************************************************************
** 函数名称: Usart_SendString
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Usart_SendString( uint32_t USARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( USARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* ȴ */
   while (usart_flag_get(USARTx ,USART_FLAG_TC)==RESET);
}

/*******************************************************************************
** 函数名称: Usart_SendHalfWord
** 功能描述: 串口初始化、收发或中断处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void Usart_SendHalfWord( uint32_t USARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ߰λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡͰλ */
	temp_l = ch&0XFF;
	
	/* ͸߰λ */
	usart_data_transmit(USARTx,temp_h);
	 while (usart_flag_get(DEBUG_USARTx ,USART_FLAG_TC)==RESET);
	
	/* ͵Ͱλ */
	usart_data_transmit(USARTx,temp_l);	
	 while (usart_flag_get(DEBUG_USARTx ,USART_FLAG_TC)==RESET);	
}


/*******************************************************************************
** 函数名称: usart_Sen_dma
** 功能描述: usart_Sen_dma函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void usart_Sen_dma(uint8_t uartx, uint8_t *buff, uint16_t len)
{
    switch (uartx)
    {
    case 0: // USART0
        dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)buff);
        dma_transfer_number_config(DMA0, DMA_CH3, len);
        dma_channel_enable(DMA0, DMA_CH3);
        break;
    case 1: // USART1
        dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)buff);
        dma_transfer_number_config(DMA0, DMA_CH6, len);
        dma_channel_enable(DMA0, DMA_CH6);
        break;
    case 2: // USART2
        dma_memory_address_config(DMA0, DMA_CH1, (uint32_t)buff);
        dma_transfer_number_config(DMA0, DMA_CH1, len);
        dma_channel_enable(DMA0, DMA_CH1);
        break;
    default:
        break;
    }
}


