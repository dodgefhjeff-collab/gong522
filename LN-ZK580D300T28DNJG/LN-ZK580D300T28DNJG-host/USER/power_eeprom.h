#ifndef __POWER_EEPROM_H
#define __POWER_EEPROM_H

#include "sys.h"

#define POWER_CLEAR_NONE            0x00
#define POWER_CLEAR_MAXMIN          0x01
#define POWER_CLEAR_WORK_HOUR       0x02
#define POWER_CLEAR_MAXMIN_AND_HOUR 0x03

/* frame[6] 子命令 */
#define POWER_EEPROM_CMD_SET_WORK_HOUR    0x01
#define POWER_EEPROM_CMD_SET_BOARD_ID     0x02
#define POWER_EEPROM_CMD_SET_MFG_DATE     0x03
#define POWER_EEPROM_CMD_CLEAR_WORK_HOUR  0x10
#define POWER_EEPROM_CMD_CLEAR_BOARD_ID   0x11
#define POWER_EEPROM_CMD_CLEAR_MFG_DATE   0x12
#define POWER_EEPROM_CMD_CLEAR_ALL        0x1F

void Power_EepromInit(void);
void Power_EepromLoad(void);
void Power_EepromSave(void);
uint8_t Power_EepromHandleSet(uint8_t sub_cmd, const uint8_t *data, uint8_t data_len);
uint8_t Power_EepromHandleClear(uint8_t sub_cmd);
void Power_EepromGetBoardId(uint8_t *buf6);
void Power_EepromGetMfgDate(uint8_t *buf3);
uint32_t Power_EepromGetWorkHour(void);
void Power_EepromSetWorkHour(uint32_t hour);

#endif
