#ifndef __CRC16_H__
#define __CRC16_H__
#include "gd32c10x.h"
#define CRC16 CRC16_Get_2
void CRC16_build(unsigned char *frame, unsigned int len);
short CRC16_Get(unsigned char *frame, unsigned int len);
unsigned int CRC16_check(unsigned char *frame, unsigned int len);
void crc16Test(void);
unsigned short getCRC_hand(unsigned char* buf, unsigned int len);
uint16_t CRC16_Get_2(unsigned char *frame, unsigned int len);

#endif
