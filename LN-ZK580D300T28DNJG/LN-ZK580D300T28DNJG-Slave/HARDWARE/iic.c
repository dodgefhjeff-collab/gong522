/*
 * iic.c
 *
 *  Created on: 2018-2-5
 *      Author: Administrator
 */

#include "iic.h"
#include "delay.h"
/*******************************************************************************
* 函 数 名         : IIC_Init
* 函数功能		   : IIC初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IICA_Init(void)
{

    rcu_periph_clock_enable(RCU_GPIOB); //GPIO时钟初始化
    gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_MAX,GPIO_PIN_14);   //引脚初始化  PB14 推挽输出
    gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_MAX,GPIO_PIN_15);   //引脚初始化  PB15 推挽输出

}

/*******************************************************************************
* 函 数 名         : SDA_OUT
* 函数功能		   : SDA输出配置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SDA_OUT(void)
{

    //GPIOB->CRH&=0X0FFFFFFF; GPIOB->CRH|=0X30000000;       //Output. SDA
    gpio_init(GPIOB,GPIO_MODE_OUT_PP,GPIO_OSPEED_MAX,GPIO_PIN_15);   //引脚初始化  PB15 推挽输出
}

/*******************************************************************************
* 函 数 名         : SDA_IN
* 函数功能		   : SDA输入配置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SDA_IN(void)
{

//    GPIOB->CRH&=0X0FFFFFFF;
//    GPIOB->CRH|=0X80000000;
//    GPIOB->BSRR = ((uint32_t)0x01)<<15;       //Input, SDA

    gpio_init(GPIOB,GPIO_MODE_IPU,GPIO_OSPEED_MAX,GPIO_PIN_15);  //引脚初始化  PB15 上拉输入
}

/*******************************************************************************
* 函 数 名         : IIC_Start
* 函数功能		   : 产生IIC起始信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA_SETH;
    IIC_SCL_SETH;
    delay_us(5);
    IIC_SDA_SETL;//START:when CLK is high,DATA change form high to low
    delay_us(6);
    IIC_SCL_SETL;//钳住I2C总线，准备发送或接收数据
}

/*******************************************************************************
* 函 数 名         : IIC_Stop
* 函数功能		   : 产生IIC停止信号
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Stop(void)
{
    SDA_OUT();     //sda线输出
    IIC_SCL_SETL;
    IIC_SDA_SETL;  //STOP:when CLK is high DATA change form low to high
    IIC_SCL_SETH;
    delay_us(6);
    IIC_SDA_SETH;  //发送I2C总线结束信号
    delay_us(6);
}

/*******************************************************************************
* 函 数 名         : IIC_Wait_Ack
* 函数功能		   : 等待应答信号到来
* 输    入         : 无
* 输    出         : 1，接收应答失败
        			 0，接收应答成功
*******************************************************************************/
unsigned char IIC_Wait_Ack(void)
{
    unsigned char tempTime=0;

    IIC_SDA_SETH;
    delay_us(1);
    SDA_IN();      //SDA设置为输入
    IIC_SCL_SETH;
    delay_us(1);
    while(READ_SDA)
    {
        tempTime++;
        if(tempTime>250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL_SETL;//时钟输出0
    return 0;
}

/*******************************************************************************
* 函 数 名         : IIC_Ack
* 函数功能		   : 产生ACK应答
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_Ack(void)
{
    IIC_SCL_SETL;
    SDA_OUT();
    IIC_SDA_SETL;
    delay_us(2);
    IIC_SCL_SETH;
    delay_us(5);
    IIC_SCL_SETL;
}

/*******************************************************************************
* 函 数 名         : IIC_NAck
* 函数功能		   : 产生NACK非应答
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void IIC_NAck(void)
{
    IIC_SCL_SETL;
    SDA_OUT();
    IIC_SDA_SETH;
    delay_us(2);
    IIC_SCL_SETH;
    delay_us(5);
    IIC_SCL_SETL;
}

/*******************************************************************************
* 函 数 名         : IIC_Send_Byte
* 函数功能		   : IIC发送一个字节
* 输    入         : txd：发送一个字节
* 输    出         : 无
*******************************************************************************/
void IIC_Send_Byte(unsigned char txd)
{
    unsigned char t;
    SDA_OUT();
    IIC_SCL_SETL;//拉低时钟开始数据传输
    for(t=0; t<8; t++)
    {
        if((txd&0x80)>0)     //0x80  1000 0000
            IIC_SDA_SETH;
        else
            IIC_SDA_SETL;
        txd<<=1;
        delay_us(2);        //对TEA5767这三个延时都是必须的
        IIC_SCL_SETH;
        delay_us(2);
        IIC_SCL_SETL;
        delay_us(2);
    }
}

/*******************************************************************************
* 函 数 名         : IIC_Read_Byte
* 函数功能		   : IIC读一个字节
* 输    入         : ack=1时，发送ACK，ack=0，发送nACK
* 输    出         : 应答或非应答
*******************************************************************************/
unsigned char IIC_Read_Byte(unsigned char ack)
{
    unsigned char i,receive=0;
    SDA_IN();                          // SDA设置为输入
    for(i=0; i<8; i++ )                //
    {
        IIC_SCL_SETL;
        delay_us(2);
        IIC_SCL_SETH;
        receive<<=1;
        if(READ_SDA)receive++;
        delay_us(1);
    }
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK
    return receive;
}

