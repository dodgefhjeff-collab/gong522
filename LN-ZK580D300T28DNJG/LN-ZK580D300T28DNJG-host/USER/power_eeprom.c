#include "power_eeprom.h"
#include "24cxx.h"
#include <string.h>

#define POWER_EEPROM_BASE_ADDR      0x0000U
#define POWER_EEPROM_MAGIC          0xA55AU
#define POWER_EEPROM_VERSION        0x01U
#define POWER_EEPROM_BLOCK_SIZE     16U

typedef struct
{
    uint16_t magic;
    uint8_t version;
    uint8_t crc;
    uint8_t board_id[6];
    uint8_t mfg_date[3];
    uint8_t work_hour[3];
    uint8_t reserved;
} power_eeprom_block_t;

static power_eeprom_block_t power_eeprom_cache;
static uint8_t power_eeprom_ready = 0;

static const uint8_t power_eeprom_default_board[6] = {0x20, 0x26, 0x01, 0x00, 0x01, 0x00};
static const uint8_t power_eeprom_default_mfg[3] = {0x20, 0x26, 0x05};

static uint8_t Power_EepromCalcCrc(const power_eeprom_block_t *block)
{
    const uint8_t *p = (const uint8_t *)block;
    uint8_t i;
    uint8_t sum = 0;

    for (i = 2; i < POWER_EEPROM_BLOCK_SIZE; i++)
    {
        sum += p[i];
    }
    return sum;
}

static void Power_EepromSetDefaults(power_eeprom_block_t *block)
{
    memset(block, 0, sizeof(*block));
    block->magic = POWER_EEPROM_MAGIC;
    block->version = POWER_EEPROM_VERSION;
    memcpy(block->board_id, power_eeprom_default_board, sizeof(block->board_id));
    memcpy(block->mfg_date, power_eeprom_default_mfg, sizeof(block->mfg_date));
    block->crc = Power_EepromCalcCrc(block);
}

static uint8_t Power_EepromIsValid(const power_eeprom_block_t *block)
{
    if (block->magic != POWER_EEPROM_MAGIC)
    {
        return 0;
    }
    if (block->version != POWER_EEPROM_VERSION)
    {
        return 0;
    }
    if (block->crc != Power_EepromCalcCrc(block))
    {
        return 0;
    }
    return 1;
}

static uint32_t Power_EepromBytesToHour(const uint8_t *bytes)
{
    return ((uint32_t)bytes[0] << 16) |
           ((uint32_t)bytes[1] << 8) |
           (uint32_t)bytes[2];
}

static void Power_EepromHourToBytes(uint32_t hour, uint8_t *bytes)
{
    if (hour > 0xFFFFFFUL)
    {
        hour = 0xFFFFFFUL;
    }
    bytes[0] = (uint8_t)((hour >> 16) & 0xFF);
    bytes[1] = (uint8_t)((hour >> 8) & 0xFF);
    bytes[2] = (uint8_t)(hour & 0xFF);
}

void Power_EepromInit(void)
{
    AT24CXX_Init();
    power_eeprom_ready = (AT24CXX_Check() == 0) ? 1U : 0U;
    Power_EepromLoad();
}

void Power_EepromLoad(void)
{
    power_eeprom_block_t temp;

    if (power_eeprom_ready == 0U)
    {
        Power_EepromSetDefaults(&power_eeprom_cache);
        return;
    }

    AT24CXX_Read(POWER_EEPROM_BASE_ADDR, (uint8_t *)&temp, POWER_EEPROM_BLOCK_SIZE);
    if (Power_EepromIsValid(&temp) == 0U)
    {
        Power_EepromSetDefaults(&power_eeprom_cache);
        Power_EepromSave();
        return;
    }

    memcpy(&power_eeprom_cache, &temp, sizeof(temp));
}

void Power_EepromSave(void)
{
    if (power_eeprom_ready == 0U)
    {
        return;
    }

    power_eeprom_cache.crc = Power_EepromCalcCrc(&power_eeprom_cache);
    AT24CXX_Write(POWER_EEPROM_BASE_ADDR, (uint8_t *)&power_eeprom_cache, POWER_EEPROM_BLOCK_SIZE);
}

uint32_t Power_EepromGetWorkHour(void)
{
    return Power_EepromBytesToHour(power_eeprom_cache.work_hour);
}

void Power_EepromSetWorkHour(uint32_t hour)
{
    Power_EepromHourToBytes(hour, power_eeprom_cache.work_hour);
}

void Power_EepromGetBoardId(uint8_t *buf6)
{
    memcpy(buf6, power_eeprom_cache.board_id, 6);
}

void Power_EepromGetMfgDate(uint8_t *buf3)
{
    memcpy(buf3, power_eeprom_cache.mfg_date, 3);
}

uint8_t Power_EepromHandleSet(uint8_t sub_cmd, const uint8_t *data, uint8_t data_len)
{
    if (data == 0)
    {
        return 0;
    }

    switch (sub_cmd)
    {
    case POWER_EEPROM_CMD_SET_WORK_HOUR:
        if (data_len < 3)
        {
            return 0;
        }
        memcpy(power_eeprom_cache.work_hour, data, 3);
        break;

    case POWER_EEPROM_CMD_SET_BOARD_ID:
        if (data_len < 6)
        {
            return 0;
        }
        memcpy(power_eeprom_cache.board_id, data, 6);
        break;

    case POWER_EEPROM_CMD_SET_MFG_DATE:
        if (data_len < 3)
        {
            return 0;
        }
        memcpy(power_eeprom_cache.mfg_date, data, 3);
        break;

    default:
        return 0;
    }

    power_eeprom_cache.magic = POWER_EEPROM_MAGIC;
    power_eeprom_cache.version = POWER_EEPROM_VERSION;
    Power_EepromSave();
    return 1;
}

uint8_t Power_EepromHandleClear(uint8_t sub_cmd)
{
    switch (sub_cmd)
    {
    case POWER_EEPROM_CMD_CLEAR_WORK_HOUR:
        memset(power_eeprom_cache.work_hour, 0, sizeof(power_eeprom_cache.work_hour));
        break;

    case POWER_EEPROM_CMD_CLEAR_BOARD_ID:
        memcpy(power_eeprom_cache.board_id, power_eeprom_default_board, sizeof(power_eeprom_cache.board_id));
        break;

    case POWER_EEPROM_CMD_CLEAR_MFG_DATE:
        memcpy(power_eeprom_cache.mfg_date, power_eeprom_default_mfg, sizeof(power_eeprom_cache.mfg_date));
        break;

    case POWER_EEPROM_CMD_CLEAR_ALL:
        Power_EepromSetDefaults(&power_eeprom_cache);
        break;

    default:
        return 0;
    }

    Power_EepromSave();
    return 1;
}
