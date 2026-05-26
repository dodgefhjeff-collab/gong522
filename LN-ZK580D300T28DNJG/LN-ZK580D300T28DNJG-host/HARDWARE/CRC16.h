#ifndef __CRC16_H__

#define __CRC16_H__
#include "gd32c10x.h"

#define CRC16 CRC16_Get_2
/* 函数声明说明：CRC16_build，CRC16_build函数，保持原工程接口并完成对应模块处理。 */
void CRC16_build(unsigned char *frame, unsigned int len);
/* 函数声明说明：CRC16_Get，CRC16_Get函数，保持原工程接口并完成对应模块处理。 */
short CRC16_Get(unsigned char *frame, unsigned int len);
/* 函数声明说明：CRC16_check，CRC16_check函数，保持原工程接口并完成对应模块处理。 */
unsigned int CRC16_check(unsigned char *frame, unsigned int len);
/* 函数声明说明：crc16Test，crc16Test函数，保持原工程接口并完成对应模块处理。 */
void crc16Test(void);
/* 函数声明说明：getCRC_hand，getCRC_hand函数，保持原工程接口并完成对应模块处理。 */
unsigned short getCRC_hand(unsigned char* buf, unsigned int len);
/* 函数声明说明：CRC16_Get_2，CRC16_Get_2函数，保持原工程接口并完成对应模块处理。 */
uint16_t CRC16_Get_2(unsigned char *frame, unsigned int len);

#endif
