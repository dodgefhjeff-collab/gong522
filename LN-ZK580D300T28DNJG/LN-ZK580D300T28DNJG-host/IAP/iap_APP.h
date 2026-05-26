#ifndef __IAP_APP_H__
#define __IAP_APP_H__
#include "gd32c10x.h"

#define IAP_Bootloat_SIZE 		0x3C00	//15K

#define PC2MCU_Rend_device_info 0x7D
#define MCU2PC_Rend_device_info 0x7B
#define PC2MCU_IAP				0x7F
#define MCU2PC_IAP 				0x7E

//该函数请放在main函数最开头
void IAP_APP_Init(void);	
void systeam_ReStart(void);
#endif







































