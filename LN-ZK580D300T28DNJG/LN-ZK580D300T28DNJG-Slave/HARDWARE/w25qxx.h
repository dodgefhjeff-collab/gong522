#ifndef __W25QXX_H
#define __W25QXX_H
/*============================= 头文件包含 ===================================*/
#include "gd32c10x.h"

/*============================= 宏定义 ======================================*/
//W25X系列/Q系列芯片列表
#define W25Q80 	0XEF13
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

#define W25QXX_CS_HIGH       gpio_bit_set(GPIOB, GPIO_PIN_12)		//W25QXX的片选信号		 
#define W25QXX_CS_LOW        gpio_bit_reset(GPIOB, GPIO_PIN_12)		//W25QXX的片选信号	
#define W25QXX_SECTOR_SIZE   4096
#define W25QXX_PAGE_SIZE   	 256
//指令表
#define W25X_WriteEnable		0x06		//写使能
#define W25X_WriteDisable		0x04	//写禁止
#define W25X_ReadStatusReg		0x05//读状态寄存器指令
#define W25X_WriteStatusReg		0x01//写状态寄存器指令
#define W25X_ReadData			0x03				//读数据
#define W25X_FastReadData		0x0B
#define W25X_FastReadDual		0x3B
#define W25X_PageProgram		0x02//页写
#define W25X_BlockErase			0xD8	//块擦除指令
#define W25X_SectorErase		0x20	//扇区擦除指令
#define W25X_ChipErase			0xC7//擦除芯片
#define W25X_PowerDown			0xB9//掉电指令
#define W25X_ReleasePowerDown	0xAB
#define W25X_DeviceID			0xAB
#define W25X_ManufactDeviceID	0x90//读ID指令
#define W25X_JedecDeviceID		0x9F


/*============================= 全局变量声明 =================================*/
extern uint16_t W25QXX_TYPE;					//定义W25QXX芯片型号
extern uint8_t W25QXX_BUFFER[W25QXX_SECTOR_SIZE];

/*============================= 函数声明 =================================*/
void w25qxx_init(void);
uint16_t  w25qxx_read_ID(void);  	    		//读取FLASH ID
uint8_t	 w25qxx_read_SR(void);        		//读取状态寄存器
void w25qxx_write_SR(uint8_t sr);  			//写状态寄存器
void w25qxx_write_enable(void);  		//写使能
void w25qxx_write_disable(void);		//写保护
void w25qxx_write_nocheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void w25qxx_read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void w25qxx_write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
void w25qxx_erase_chip(void);    	  	//整片擦除
void w25qxx_erase_sector(uint32_t Dst_Addr);	//扇区擦除
void w25qxx_wait_busy(void);           	//等待空闲
void w25qxx_powerdown(void);        	//进入掉电模式
void w25qxx_wakeup(void);				//唤醒
void w25qxx_spiflashsem_create(void);
#endif
















