#ifndef _CRC_H
#define _CRC_H

#include "sys.h"


unsigned int CRC32Calculate(uint8_t *pBuf ,uint16_t pBufSize);
void CRC32TableCreate(void);
uint8_t check_data(uint8_t *pBuf ,uint16_t pBufSize);	//1:³É¹¦ 0:´íÎó 






#endif


