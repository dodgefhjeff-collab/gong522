/*
说明：标准IIC协议传输数据时为MSB方式，即高位在前低位在后，但有些器件为LSB方式，
即低位在前，高位在后，如TM1637数码管驱动芯片。
*/


//#define IIC_LSB//定义了则IIC在数据传输时低位在前


/*******************************************************************************
  * 函数名：Port_SetMode
  * 功  能：GPIO设置输入或输出模式
  * 参  数：*GPIOx 引脚组号
			GPIO_Pin引脚号
			u32Mode输入或输出模式
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void Port_SetMode(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint32_t u32Mode)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_Pin;
    GPIO_InitStruct.Mode = u32Mode;
    //GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}



/*******************************************************************************
  * 函数名：IIC_Start
  * 功  能：起始信号
  * 参  数：无
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void IIC_Start(void)
{
    IIC_SdaModeOut();
    IIC_SdaOutput_H();
    IIC_SclOutput_H();
    delay_us(5);            //>4.7us
    IIC_SdaOutput_L();
    delay_us(4);            //>4us
    IIC_SclOutput_L();
}
/*******************************************************************************
  * 函数名：IIC_Stop
  * 功  能：结束信号
  * 参  数：无
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void IIC_Stop(void)
{
    IIC_SdaModeOut();
    IIC_SclOutput_L();
    IIC_SdaOutput_L();
    IIC_SclOutput_H();
    delay_us(5);            //>4us
    IIC_SdaOutput_H();
    delay_us(4);            //>4.7us
    IIC_SdaOutput_L();
}
/*******************************************************************************
  * 函数名：IIC_Ack
  * 功  能：应答信号
  * 参  数：无
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void IIC_Ack(void)
{
    IIC_SclOutput_L();
    IIC_SdaModeOut();
    IIC_SdaOutput_L();
    delay_us(2);
    IIC_SclOutput_H();
    delay_us(4);            //>4us
    IIC_SclOutput_L();
}
/*******************************************************************************
  * 函数名：IIC_NoAck
  * 功  能：非应答信号
  * 参  数：无
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void IIC_NoAck(void)
{
    IIC_SclOutput_L();
    IIC_SdaModeOut();
    IIC_SdaOutput_H();
    delay_us(4);
    IIC_SclOutput_H();
    delay_us(4);                //>4us
    IIC_SclOutput_L();
}
/*******************************************************************************
  * 函数名：IIC_WaitAck
  * 功  能：等待应答信号
  * 参  数：无
  * 返回值：0应答成功，1应答失败
  * 说  明：从机把总线拉低，为应答成功
*******************************************************************************/
uint8_t IIC_WaitAck(void)
{
    uint8_t u8ErrCnt = 0;
    IIC_SdaModeIn();            //输入状态
    IIC_SdaOutput_H();
    IIC_SclOutput_H();
    while (IIC_SdaRead() == 1)
    {
        u8ErrCnt++;
        if (u8ErrCnt > 250)
        {
            IIC_Stop();        //发送停止信号
            return 1;
        }
    }
    IIC_SclOutput_L();
    return 0;
}
/*******************************************************************************
  * 函数名：IIC_WriteByte
  * 功  能：SDA线上输出一个字节
  * 参  数：u8Data需要写入的数据
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void IIC_WriteByte(uint8_t u8Data)
{
    uint8_t i;
    uint8_t u8Temp;
    IIC_SdaModeOut();
    IIC_SclOutput_L();
    for (i = 0; i < 8; i++)
    {
        delay_us(2);
#ifdef IIC_LSB                            //低位在前
        u8Temp = ((u8Data << (7 - i)) & 0x80);
        (u8Temp == 0x80) ? (IIC_SdaOutput_H()) : (IIC_SdaOutput_L());
#else                                     //高位在前
        u8Temp = ((u8Data >> (7 - i)) & 0x01);
        (u8Temp == 0x01) ? (IIC_SdaOutput_H()) : (IIC_SdaOutput_L());
#endif
        IIC_SclOutput_H();                          //时钟保持高电平
        delay_us(2);
        IIC_SclOutput_L();                          //时钟拉低，才允许SDA变化
    }
}
/*******************************************************************************
  * 函数名：IIC_ReadByte
  * 功  能：读一个字节
  * 参  数：无
  * 返回值：读出的数据
  * 说  明：无
*******************************************************************************/
uint8_t IIC_ReadByte(void)
{
    uint8_t i;
    uint8_t bit = 0;
    uint8_t data = 0;
    IIC_SdaModeIn();            //输入状态
    for (i = 0; i < 8; i++)
    {
        IIC_SclOutput_L();
        delay_us(2);
        IIC_SclOutput_H();
        bit = IIC_SdaRead();    //读出1位
#ifdef IIC_LSB          //低位在前
        data |= (bit << i);
#else                   //高位在前
        data = (data << 1) | bit;
#endif
        delay_us(2);
    }
    return data;
}


/*******************************************************************************
  * 函数名：x24Cxx_WriteByte
  * 功  能：写一个字节
  * 参  数：u16Addr要写入的地址
			u8Data要写入的数据
  * 返回值：无
  * 说  明：无
*******************************************************************************/
void x24Cxx_WriteByte(uint16_t u16Addr, uint8_t u8Data)
{
    x24Cxx_WriteEnable();//使能写入
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | WRITE_CMD | (((uint8_t)((u16Addr >> 8) & 0x07)) << 1));//器件寻址+写+页选择位
    IIC_WaitAck();//等待应答
    IIC_WriteByte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节，高字节如果有，已经按照页选择位处理过了
    IIC_WaitAck();//等待应答
    IIC_WriteByte(u8Data);
    IIC_WaitAck();//等待应答
    IIC_Stop();//停止信号
    x24Cxx_WriteDisble();//禁止写入
}


/*******************************************************************************
  * 函数名：x24Cxx_WritePage
  * 功  能：页写
  * 参  数：u16Addr要写入的首地址；
			u8Len写入数据字节数，最大为PAGE_SIZE
			pData要写入的数据首地址
  * 返回值：无
  * 说  明：最多写入1页，防止翻卷，如果地址跨页则去掉跨页的部分
*******************************************************************************/
void x24Cxx_WritePage(uint16_t u16Addr, uint8_t u8Len, uint8_t *pData)
{
    uint8_t i;
    x24Cxx_WriteEnable();//使能写入
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | WRITE_CMD | (((uint8_t)((u16Addr >> 8) & 0x07)) << 1));//器件寻址+写+页选择位
    IIC_WaitAck();//等待应答
    IIC_WriteByte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节，高字节如果有，已经按照页选择位处理过了
    IIC_WaitAck();//等待应答
    if (u8Len > PAGE_SIZE)//长度大于页的长度
    {
        u8Len = PAGE_SIZE;
    }
    if ((u16Addr + (uint16_t)u8Len) > CAPACITY_SIZE)//超过容量
    {
        u8Len = (uint8_t)(CAPACITY_SIZE - u16Addr);
    }
    if (((u16Addr % PAGE_SIZE) + (uint16_t)u8Len) > PAGE_SIZE)//判断是否跨页
    {
        u8Len -= (uint8_t)((u16Addr + (uint16_t)u8Len) % PAGE_SIZE);//跨页，截掉跨页的部分
    }

    for (i = 0; i < u8Len; i++)
    {
        IIC_WriteByte(*(pData + i));
        IIC_WaitAck();//等待应答
    }
    IIC_Stop();//停止信号
    x24Cxx_WriteDisble();//禁止写入
}

/*******************************************************************************
  * 函数名：x24Cxx_ReadByte
  * 功  能：读一个字节
  * 参  数：u16Addr要读取的地址
  * 返回值：u8Data读出的数据
  * 说  明：无
*******************************************************************************/
uint8_t x24Cxx_ReadByte(uint16_t u16Addr)
{
    uint8_t u8Data = 0;
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | WRITE_CMD | (((uint8_t)((u16Addr >> 8) & 0x07)) << 1));//器件寻址+写+页选择位
    IIC_WaitAck();//等待应答
    IIC_WriteByte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节，高字节如果有，已经按照页选择位处理过了
    IIC_WaitAck();//等待应答
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | READ_CMD);//器件寻址+读
    IIC_WaitAck();//等待应答
    u8Data = IIC_ReadByte();
    IIC_NoAck();
    IIC_Stop();//停止信号
    return u8Data;
}

/*******************************************************************************
  * 函数名：x24Cxx_ReadPage
  * 功  能：页读
  * 参  数：u16Addr要读取的首地址；
			u8Len读取数据字节数，最大为PAGE_SIZE
			pBuff读取数据存入的缓存
  * 返回值：无
  * 说  明：最多读1页，防止翻卷，如果地址跨页则去掉跨页的部分
*******************************************************************************/
void x24Cxx_ReadPage(uint16_t u16Addr, uint8_t u8Len, uint8_t *pBuff)
{
    uint8_t i;
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | WRITE_CMD | (((uint8_t)((u16Addr >> 8) & 0x07)) << 1));//器件寻址+写+页选择位
    IIC_WaitAck();//等待应答
    IIC_WriteByte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节，高字节如果有，已经按照页选择位处理过了
    IIC_WaitAck();//等待应答
    IIC_Start();//起始信号
    IIC_WriteByte(DEV_ADDR | READ_CMD);//器件寻址+读
    IIC_WaitAck();//等待应答
    if (u8Len > PAGE_SIZE)//长度大于页的长度
    {
        u8Len = PAGE_SIZE;
    }
    if ((u16Addr + (uint16_t)u8Len) > CAPACITY_SIZE)//超过容量
    {
        u8Len = (uint8_t)(CAPACITY_SIZE - u16Addr);
    }
    if (((u16Addr % PAGE_SIZE) + (uint16_t)u8Len) > PAGE_SIZE)//判断是否跨页
    {
        u8Len -= (uint8_t)((u16Addr + (uint16_t)u8Len) % PAGE_SIZE);//跨页，截掉跨页的部分
    }
    for (i = 0; i < (u8Len - 1); i++)
    {
        *(pBuff + i) = IIC_ReadByte();
        IIC_Ack();//主机应答
    }
    *(pBuff + u8Len - 1) = IIC_ReadByte();
    IIC_NoAck();//最后一个不应答
    IIC_Stop();//停止信号
}
