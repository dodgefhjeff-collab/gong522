

/*============================= 头文件包含 =================================*/
#include "w25qxx.h"
#include "delay.h"
#include "sys.h"
#include "gd_spi.h"
/*============================= 全局变量定义 =================================*/
uint16_t W25QXX_TYPE = W25Q64;	//W25Q34
//256bytes/page(can be programmed at a time)
//4Kbytes/Sector(can be erased)
//32Kbytes/Block(can be erased)
//64Kbytes/Block(can be erased)
//W25Q32
//容量为32Mbits=4MBites,共有64个Block,1024个Sector
//– 104MHz Single, Dual/Quad SPI clocks
//– 208/416MHz equivalent Dual/Quad SPI
//– 50MB/S continuous data transfer rate


/*============================= 函数定义 =================================*/
/*
************************************************************
*	函数名称：	w25qxx_init
*	说明：		初始化spi口、读取spi_flash的出厂ID
************************************************************
*/
void w25qxx_init(void)
{
    gd_spi_init();		   			//初始化SPI
    W25QXX_CS_HIGH;					//SPI FLASH不选中
    W25QXX_TYPE = w25qxx_read_ID();	//读取FLASH ID.
    w25qxx_write_enable();

}

/*
************************************************************
*	函数名称：	w25qxx_read_SR
*	说明：		读取W25QXX的状态寄存器
*				BIT7  6   5   4   3   2   1   0
*				SPR   RV  TB BP2 BP1 BP0 WEL BUSY
*				SPR:默认0,状态寄存器保护位,配合WP使用
*				TB,BP2,BP1,BP0:FLASH区域写保护设置
*				WEL:写使能锁定
*				BUSY:忙标记位(1,忙;0,空闲)
*				默认:0x00
************************************************************
*/
uint8_t w25qxx_read_SR(void)
{
    uint8_t byte = 0;
    W25QXX_CS_LOW;                            //使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_ReadStatusReg); //发送读取状态寄存器命令
    byte = spi1_readwrite_byte(0Xff);          //读取一个字节
    W25QXX_CS_HIGH;                            //取消片选
    return byte;
}

/*
************************************************************
*	函数名称：	w25qxx_write_SR
*	说明：		写W25QXX状态寄存器
*				只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
************************************************************
*/
void w25qxx_write_SR(uint8_t sr)
{
    W25QXX_CS_LOW;                            //使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_WriteStatusReg);//发送写取状态寄存器命令
    spi1_readwrite_byte(sr);               	//写入一个字节
    W25QXX_CS_HIGH;                            //取消片选
}

/*
************************************************************
*	函数名称：	w25qxx_write_enable
*	说明：		W25QXX写使能	将WEL置位
************************************************************
*/
void w25qxx_write_enable(void)
{
    W25QXX_CS_LOW;                          	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_WriteEnable); 	//发送写使能
    W25QXX_CS_HIGH;                          	//取消片选
}

/*
************************************************************
*	函数名称：	w25qxx_write_enable
*	说明：		W25QXX写禁止	  将WEL清零
************************************************************
*/
void w25qxx_write_disable(void)
{
    W25QXX_CS_LOW;                            //使能器件
    spi1_readwrite_byte(W25X_WriteDisable);  //发送写禁止指令
    W25QXX_CS_HIGH;                           //取消片选
}

/*
************************************************************
*	函数名称：	w25qxx_write_enable
*	说明：		读取芯片ID
*				0XEF13,表示芯片型号为W25Q80
*				0XEF14,表示芯片型号为W25Q16
*				0XEF15,表示芯片型号为W25Q32
*				0XEF16,表示芯片型号为W25Q64
*				0XEF17,表示芯片型号为W25Q128
************************************************************
*/
uint16_t w25qxx_read_ID(void)
{
    uint16_t Temp = 0;
    W25QXX_CS_LOW;
    delay_us(30);
    spi1_readwrite_byte(W25X_ManufactDeviceID);//发送读取ID命令
    spi1_readwrite_byte(0x00);
    spi1_readwrite_byte(0x00);
    spi1_readwrite_byte(0x00);
//	spi1_readwrite_byte(W25X_DeviceID);//发送读取ID命令
//	spi1_readwrite_byte(0xFF);
//	spi1_readwrite_byte(0xFF);
//	spi1_readwrite_byte(0xFF);
    Temp |= spi1_readwrite_byte(0xFF)<<8;
    Temp |= spi1_readwrite_byte(0xFF);
    W25QXX_CS_HIGH;
    return Temp;
}

/*
************************************************************
*	函数名称：	w25qxx_read
*	说明：		在指定地址开始读取指定长度的数据
*				pBuffer:数据存储区
*				ReadAddr:开始读取的地址值
*				NumByteToRead:要读取的字节数
************************************************************
*/
void w25qxx_read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
    uint16_t i;
    W25QXX_CS_LOW;                            	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_ReadData);         //发送读取命令
    spi1_readwrite_byte((uint8_t)((ReadAddr)>>16));  //发送24bit地址
    spi1_readwrite_byte((uint8_t)((ReadAddr)>>8));
    spi1_readwrite_byte((uint8_t)ReadAddr);
    for(i = 0; i < NumByteToRead; i++)
    {
        pBuffer[i] = spi1_readwrite_byte(0XFF); //循环读数
    }
    W25QXX_CS_HIGH;
}

/*
************************************************************
*	函数名称：	w25qxx_write_page
*	说明：		SPI在一页(0~65535)内写入少于256个字节的数据
*				在指定地址开始写入最大256字节的数据
*				pBuffer:数据存储区
*				ReadAddr:开始读取的地址(24bit)
*				NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
************************************************************
*/
void w25qxx_write_page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t i;
    w25qxx_write_enable();                  	//SET WEL
    W25QXX_CS_LOW;                            	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_PageProgram);      	//发送写页命令
    spi1_readwrite_byte((uint8_t)((WriteAddr)>>16)); 	//发送24bit地址
    spi1_readwrite_byte((uint8_t)((WriteAddr)>>8));
    spi1_readwrite_byte((uint8_t)WriteAddr);
    for(i = 0; i < NumByteToWrite; i++)
        spi1_readwrite_byte(pBuffer[i]);//循环写数
    W25QXX_CS_HIGH;                            	//取消片选
    w25qxx_wait_busy();					   		//等待写入结束
}


/*
************************************************************
*	函数名称：	w25qxx_write_nocheck
*	说明：		无检验写SPI FLASH
*				必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*				具有自动换页功能
*				在指定地址开始写入指定长度的数据,但是要确保地址不越界!
*				pBuffer:数据存储区
*				WriteAddr:开始写入的地址(24bit)
*				NumByteToWrite:要写入的字节数(最大65535)
*				CHECK OK
************************************************************
*/
void w25qxx_write_nocheck(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t pageremain;
    pageremain = W25QXX_PAGE_SIZE - WriteAddr%W25QXX_PAGE_SIZE; //单页剩余的字节数
    if(NumByteToWrite <= pageremain)
        pageremain = NumByteToWrite;//不大于256个字节
    while(1)
    {
        w25qxx_write_page(pBuffer, WriteAddr, pageremain);
        if(NumByteToWrite == pageremain)
            break;//写入结束了
        else //NumByteToWrite>pageremain
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;

            NumByteToWrite -= pageremain;			  //减去已经写入了的字节数
            if(NumByteToWrite > W25QXX_PAGE_SIZE)
                pageremain = W25QXX_PAGE_SIZE; //一次可以写入256个字节
            else
                pageremain = NumByteToWrite; 	  //不够256个字节了
        }
    }
}

/*
************************************************************
*	函数名称：	w25qxx_write
*	说明：		在指定地址开始写入指定长度的数据
*				pBuffer:数据存储区
*				WriteAddr:开始写入的地址
*				NumByteToWrite:要写入的字节数
************************************************************
*/
uint8_t W25QXX_BUFFER[W25QXX_SECTOR_SIZE];
void w25qxx_write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    uint8_t * W25QXX_BUF;
    W25QXX_BUF = W25QXX_BUFFER;
    secpos = WriteAddr/W25QXX_SECTOR_SIZE;	//扇区地址
    secoff = WriteAddr%W25QXX_SECTOR_SIZE;	//在扇区内的偏移
    secremain = W25QXX_SECTOR_SIZE - secoff;//扇区剩余空间大小
    if(NumByteToWrite <= secremain)
        secremain = NumByteToWrite;			//不大于W25QXX_SECTOR_SIZE个字节
    while(1)
    {
        w25qxx_read(W25QXX_BUF, secpos*W25QXX_SECTOR_SIZE, W25QXX_SECTOR_SIZE);
        for(i = 0; i < secremain; i++)
        {
            if(W25QXX_BUF[secoff+i] != 0XFF)
                break;
        }
        if(i < secremain)					//需要擦除
        {
            w25qxx_erase_sector(secpos);	//擦除这个扇区
            for(i = 0; i < secremain; i++)
            {
                W25QXX_BUF[i+secoff] = pBuffer[i];
            }
            w25qxx_write_nocheck(W25QXX_BUF, secpos*W25QXX_SECTOR_SIZE, W25QXX_SECTOR_SIZE);
        }									//写入整个扇区
        else
            w25qxx_write_nocheck(pBuffer, WriteAddr, secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(NumByteToWrite == secremain)
            break;							//写入结束了
        else								//写入未结束
        {
            secpos++;						//扇区地址增1
            secoff = 0;						//偏移位置为0
            pBuffer += secremain;  			//指针偏移
            WriteAddr += secremain;			//写地址偏移
            NumByteToWrite -= secremain;	//字节数递减
            if(NumByteToWrite > W25QXX_SECTOR_SIZE)
                secremain = W25QXX_SECTOR_SIZE;	//下一个扇区还是写不完
            else
                secremain = NumByteToWrite;		//下一个扇区可以写完了
        }
    }
}

/*
************************************************************
*	函数名称：	w25qxx_erase_sector
*	说明：		擦除整个芯片
*				等待时间超长...
************************************************************
*/
void w25qxx_erase_chip(void)
{
    w25qxx_write_enable();                 	 	//SET WEL
    w25qxx_wait_busy();
    W25QXX_CS_LOW;                            	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_ChipErase);        	//发送片擦除命令
    W25QXX_CS_HIGH;                            	//取消片选
    w25qxx_wait_busy();   				   		//等待芯片擦除结束
}

/*
************************************************************
*	函数名称：	w25qxx_erase_sector
*	说明：		擦除一个扇区
*				Dst_Addr:扇区号 根据实际容量设置
*				擦除一个扇区的最少时间:150ms
************************************************************
*/
void w25qxx_erase_sector(uint32_t Dst_Addr)
{
    //监视falsh擦除情况,测试用
    Dst_Addr *= W25QXX_SECTOR_SIZE;
    w25qxx_write_enable();                  	//SET WEL
    w25qxx_wait_busy();
    W25QXX_CS_LOW;                            	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_SectorErase);      	//发送扇区擦除指令
    spi1_readwrite_byte((uint8_t)((Dst_Addr)>>16));  	//发送24bit地址
    spi1_readwrite_byte((uint8_t)((Dst_Addr)>>8));
    spi1_readwrite_byte((uint8_t)Dst_Addr);
    W25QXX_CS_HIGH;                            	//取消片选
    w25qxx_wait_busy();   				   		//等待擦除完成
}

/*
************************************************************
*	函数名称：	w25qxx_wait_busy
*	说明：		等待空闲
************************************************************
*/
void w25qxx_wait_busy(void)
{
    while((w25qxx_read_SR()&0x01) == 0x01);  		// 等待BUSY位清空
}

/*
************************************************************
*	函数名称：	w25qxx_powerdown
*	说明：		进入掉电模式
************************************************************
*/
void w25qxx_powerdown(void)
{
    W25QXX_CS_LOW;                           	 	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_PowerDown);        //发送掉电命令
    W25QXX_CS_HIGH;                            	//取消片选
    delay_us(3);                               //等待TPD
}

/*
************************************************************
*	函数名称：	w25qxx_wakeup
*	说明：		唤醒
************************************************************
*/
void w25qxx_wakeup(void)
{
    W25QXX_CS_LOW;                            	//使能器件
    delay_us(30);
    spi1_readwrite_byte(W25X_ReleasePowerDown);	//  send W25X_PowerDown command 0xAB
    W25QXX_CS_HIGH;                            	//取消片选
    delay_us(3);                            	//等待TRES1
}
























