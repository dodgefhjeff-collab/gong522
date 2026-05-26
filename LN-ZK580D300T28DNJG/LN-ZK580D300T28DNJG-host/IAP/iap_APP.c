#include "iap_APP.h"

void systeam_ReStart(void)
{
	uint32_t time = 65535;
	while(time--)
	{
		__NOP();
	}
	__set_FAULTMASK(1);
	NVIC_SystemReset();
}

void IAP_APP_Init(void)
{
	SCB->VTOR = FLASH_BASE | IAP_Bootloat_SIZE; //IAP_Bootloat_SIZE:0x5000=20K ×Ö½Ú
}
