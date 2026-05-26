#ifndef __GD_SPI_H
#define	__GD_SPI_H
#include "gd32c10x.h"


uint8_t spi1_readwrite_byte(uint8_t TxData);
void gd_spi_init(void);
void spi_rcu_config(void);
void  spi_gpio_config(void);
void  spi_dma_config(void);
void spi_config(void);
#endif


