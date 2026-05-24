#include "iap_crc.h"

//查表
unsigned long CRC32Table[256];

void CRC32TableCreate(void)
{
    unsigned int c;
    unsigned int i, j;

    for (i = 0; i < 256; i++) {
        c = (unsigned int)i;
        for (j = 0; j < 8; j++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        CRC32Table[i] = c;
    }

}

unsigned int CRC32Calculate(uint8_t *pBuf ,uint16_t pBufSize)
{
    unsigned int retCRCValue=0xffffffff;
    unsigned char *pData;
    pData=(unsigned char *)pBuf;
     while(pBufSize--)
     {
         retCRCValue=CRC32Table[(retCRCValue ^ *pData++) & 0xFF]^ (retCRCValue >> 8);
     }
     return retCRCValue^0xffffffff;
}


uint8_t check_data(uint8_t *pBuf ,uint16_t pBufSize)	//1:成功 0:错误 
{
	uint32_t get_PC_crcVel=0;
    uint32_t CRC_PC_vel = 0;
	uint8_t *pbuff;
	if(pBufSize<5)
    {
        return 0;
    }
	pbuff = pBuf+pBufSize-4;
    
	CRC_PC_vel = pbuff[0]<<24|pbuff[1]<<16|pbuff[2]<<8|pbuff[3];	//接收的CRC32
	
	get_PC_crcVel 	 = CRC32Calculate(pBuf,pBufSize-4);	//MCU端计算的CRC32
	
	if(get_PC_crcVel == CRC_PC_vel)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



//一直算
uint16_t CalcCrc(unsigned char *chData, unsigned short uNo)
{
	uint16_t crc = 0xffff;
	uint16_t i, j;
	for(i = 0; i < uNo; i++)
	{
		crc ^= chData[i];
		for(j = 0; j < 8; j++)
		{
			if(crc & 1)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	return (crc);
}





