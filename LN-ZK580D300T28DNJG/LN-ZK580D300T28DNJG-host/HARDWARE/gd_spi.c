#include "gd32c10x.h"
#include "gd_spi.h"

#define SPI_CRC_ENABLE           1
#define ARRAYSIZE                10
uint8_t spi1_send_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};

uint8_t spi1_receive_array[ARRAYSIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA};
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gd_spi_init(void)
{
    /* enable peripheral clock */
    spi_rcu_config();
    /* configure GPIO */
    spi_gpio_config();
    /* configure DMA */
    //spi_dma_config();
    /* configure SPI */
    spi_config();

    /* enable SPI */
    


    /* enable SPI DMA */


}

/*!
    \brief      configure different peripheral clocks
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_rcu_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);
   // rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI1);
}

/*!
    \brief      configure the GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_gpio_config(void)
{
    /* SPI1 GPIO config:SCK/PB13, MISO/PB14, MOSI/PB15 ,NSS PB12*/
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_15 | GPIO_PIN_13 );
		gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_14);
}

/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    dma_struct_para_init(&dma_init_struct);

    /* configure SPI1 transmit DMA:DMA0-DMA_CH4 */
    dma_deinit(DMA0, DMA_CH4);

    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);			//外设地址
    dma_init_struct.memory_addr  = (uint32_t)spi1_send_array;			//内存地址
    dma_init_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;			//传输方向
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;					//内存数据宽度
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;			//外设地址宽度
    dma_init_struct.priority     = DMA_PRIORITY_LOW;							//优先级
    dma_init_struct.number       = ARRAYSIZE;											//传输数量
    dma_init_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;		//外设地址生成算法
    dma_init_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;		//内存地址生成算法
    dma_init(DMA0, DMA_CH4, &dma_init_struct);										//DMA初始化
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH4);												//循环模式禁止
    dma_memory_to_memory_disable(DMA0, DMA_CH4);									//内存到内存模式禁止

		dma_channel_disable(DMA0, DMA_CH4);

    /* configure SPI1 receive DMA: DMA0-DMA_CH3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.periph_addr  = (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.memory_addr  = (uint32_t)spi1_receive_array;
    dma_init_struct.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);
		
		dma_channel_enable(DMA0, DMA_CH3);
		
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
    spi_dma_enable(SPI1, SPI_DMA_RECEIVE);
}

/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitialize SPI and the parameters */
    spi_i2s_deinit(SPI1);
    spi_struct_para_init(&spi_init_struct);

    /* configure SPI1 parameter */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;			//传输模式
    spi_init_struct.device_mode          = SPI_MASTER;										//设备模式配置
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;						//数据帧格式配置
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE ;				//相位和极性配置
    spi_init_struct.nss                  = SPI_NSS_SOFT ;									//NSS由软件或硬件控制配置
    spi_init_struct.prescale             = SPI_PSC_256;										//预分频器配置
    spi_init_struct.endian               = SPI_ENDIAN_MSB;								//大端或小端模式配置
    spi_init(SPI1, &spi_init_struct);																			//SPI初始化
	
		spi_enable(SPI1);																											//SPI使能
}

uint8_t spi1_readwrite_byte(uint8_t TxData)
{		
	uint8_t retry=0;				 	
	while (spi_i2s_flag_get(SPI1, SPI_FLAG_TBE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
	{
		retry++;
		if(retry>200)
			return 0;
	}			  
	spi_i2s_data_transmit(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (spi_i2s_flag_get(SPI1, SPI_FLAG_RBNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
	{
		retry++;
		if(retry>200)
			return 0;
	}	  						    
	return spi_i2s_data_receive(SPI1); //返回通过SPIx最近接收的数据					    
}

