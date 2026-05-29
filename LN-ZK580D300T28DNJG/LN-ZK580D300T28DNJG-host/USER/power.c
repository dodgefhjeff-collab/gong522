#include "power.h"
#include "power_eeprom.h"

#include "adc.h"
#include "ntc_temp_table.h"
#include <string.h>

/* -------------------- 故障/告警状态说明 -------------------- */

uint16_t fatal_error_flag = 0;
uint8_t error_flag = 0;
uint8_t tim2_up_flag = 0;
uint16_t ADC_results[M];
uint8_t vo1_shutdown_flag = 0;
uint8_t vo2_shutdown_flag = 0;
uint8_t power_fan_switch = POWER_FAN_OFF;
uint8_t power_fan_duty = 0x00;
uint8_t power_self_check_type = POWER_SELF_POWERON;
uint8_t power_self_check_state = 0x00;
static uint16_t power_vin_u_max = 0;
static uint16_t power_vin_u_min = 0xFFFF;
static uint16_t power_12v_u_max = 0;
static uint16_t power_12v_u_min = 0xFFFF;
static uint16_t power_28v_u_max = 0;
static uint16_t power_28v_u_min = 0xFFFF;
static uint16_t power_f28v_u_max = 0;
static uint16_t power_f28v_u_min = 0xFFFF;
static uint16_t power_vin_i_max = 0;
static uint16_t power_vin_i_min = 0xFFFF;
static uint32_t power_work_hour = 0;
static uint32_t power_work_ms_cnt = 0;

#if POWER_SELF_CHECK_AUTO_REPORT_ENABLE


static uint32_t power_report_ms_cnt = 0;
#endif

uint16_t power_slave_f28v_u = 0;
uint16_t power_slave_fan_speed = 0;
uint16_t power_slave_i_f1 = 0;
uint8_t power_slave_y = 0;
uint16_t power_slave_f_pwm = 0;
uint16_t power_slave_s0 = 0;
uint16_t power_slave_s1 = 0;
uint16_t power_slave_s2 = 0;
uint16_t power_slave_temp = 0;
uint8_t power_slave_online = 0;
static uint32_t power_slave_rx_cnt = 0;

/* -------------------- 0x0400 / 0x040F / 0x041F框架状态缓存 -------------------- */
/* 0x0400为外部管理软件到电源模块的ACK/NAK应答。；当前应用层暂时没有必须等待0400确认的主动设置类消息，；但仍完整解析并保存字段，便于后续主动上报或升级流程使用。 */

uint8_t power_host_ack_valid = 0;
uint16_t power_host_ack_msg_id = 0;
uint8_t power_host_ack_nak = 0;
uint8_t power_host_ack_err_code = 0;
uint8_t power_host_ack_set_flag = 0;

/* 0x040F为外部管理软件对0x041F软件升级报告的应答。；当前Flash擦写/回退由Bootloader接入后完成，应用层先保存应答字段，；为完整在线升级状态机预留闭环入口。 */

uint8_t power_update_report_ack_valid = 0;
uint8_t power_update_report_ack_value = 0;
uint16_t power_update_report_ack_crc = 0;
extern xSemaphoreHandle FillBuffMutex;
extern int16_t wendu;

/* Power_FillU16：电源模块业务处理函数 */
static void Power_FillU16(uint8_t *buff, uint16_t index, uint16_t data)
{
    buff[index] = (uint8_t)((data >> 8) & 0xFF);
    buff[index + 1] = (uint8_t)(data & 0xFF);
}

/* Power_ReadU16LE：电源模块业务处理函数 */
static uint16_t Power_ReadU16LE(uint8_t *buff)
{
    return ((uint16_t)buff[1] << 8) | buff[0];
}


/* Power_EncodeTemperature：电源模块业务处理函数 */
static uint8_t Power_EncodeTemperature(int16_t temp)
{
    
    uint8_t abs_temp;

    if (temp < 0)
    {
        temp = -temp;
        if (temp > 127)
        {
            temp = 127;
        }
        abs_temp = (uint8_t)temp;
        return (uint8_t)(0x80 | abs_temp);
    }

    if (temp > 127)
    {
        temp = 127;
    }
    return (uint8_t)temp;
}

/* Power_UpdateMaxMin：电源模块业务处理函数 */
static void Power_UpdateMaxMin(uint16_t data, uint16_t *max, uint16_t *min)
{
    if (data > *max)
    {
        *max = data;
    }
    if (data < *min)
    {
        *min = data;
    }
}

/* Power_HandleSlaveFrame：电源模块业务处理函数 */
static void Power_HandleSlaveFrame(uint8_t *buff)
{
    power_slave_f28v_u = Power_ReadU16LE(&buff[2]);
    power_slave_fan_speed = Power_ReadU16LE(&buff[4]);
    power_slave_i_f1 = Power_ReadU16LE(&buff[6]);
    power_slave_y = buff[8];
    power_slave_f_pwm = Power_ReadU16LE(&buff[9]);
    power_slave_s0 = Power_ReadU16LE(&buff[11]);
    power_slave_s1 = Power_ReadU16LE(&buff[13]);
    power_slave_s2 = Power_ReadU16LE(&buff[15]);
    power_slave_temp = Power_ReadU16LE(&buff[17]);

    power_slave_online = 1;
    power_slave_rx_cnt++;

    /* 28PIN上传的F28VU作为主机0x0412里的“输出28V电压值（信道）”来源。；因此这里同时刷新28V信道当前统计和整机风机28V统计，保证最大/最小值都跟随28PIN实测值。 */
    Power_UpdateMaxMin(power_slave_f28v_u, &power_28v_u_max, &power_28v_u_min);
    Power_UpdateMaxMin(power_slave_f28v_u, &power_f28v_u_max, &power_f28v_u_min);
}

/* Power_HandleSlaveByte：电源模块业务处理函数 */
static void Power_HandleSlaveByte(uint8_t ch)
{
    
    static uint8_t rx_buf[POWER_SLAVE_UPLOAD_LEN];
    
    static uint8_t rx_cnt = 0;

    if (rx_cnt == 0)
    {
        if (ch == POWER_SLAVE_FRAME_HEAD1)
        {
            rx_buf[rx_cnt++] = ch;
        }
        return;
    }

    if (rx_cnt == 1)
    {
        if (ch == POWER_SLAVE_FRAME_HEAD2)
        {
            rx_buf[rx_cnt++] = ch;
        }
        else if (ch == POWER_SLAVE_FRAME_HEAD1)
        {
            rx_cnt = 1;
            rx_buf[0] = ch;
        }
        else
        {
            rx_cnt = 0;
        }
        return;
    }

    rx_buf[rx_cnt++] = ch;
    if (rx_cnt >= POWER_SLAVE_UPLOAD_LEN)
    {
        if (rx_buf[19] == POWER_SLAVE_FRAME_TAIL1 && rx_buf[20] == POWER_SLAVE_FRAME_TAIL2)
        {
            Power_HandleSlaveFrame(rx_buf);
        }
        rx_cnt = 0;
    }
}

/* Power_ProcessSlaveUpload：解析28PIN从机通过PA2/PA3上传的固定长度采集帧 */
/* Power_ProcessSlaveUpload：从PA2/PA3内部串口环形缓冲取出字节，解析28PIN上传的55 AA ... 66 BB采集帧 */
void Power_ProcessSlaveUpload(void)
{
    
    uint8_t ch;

    while (uart_get_rx_byte(&ch) != 0)
    {
        Power_HandleSlaveByte(ch);
    }
}

/* Power_SendSlaveControl：48PIN收到外部0x0403控制命令后，通过PA2/PA3下发给28PIN */
/* Power_SendSlaveControl：48PIN收到外部0x0403控制命令后，通过PA2/PA3向28PIN下发55 AA C1 ... 66 BB控制帧 */
void Power_SendSlaveControl(uint8_t fan_switch, uint8_t duty_code, uint8_t clear_flag)
{
    
    uint8_t buff[POWER_SLAVE_CMD_LEN];

    memset(buff, 0, sizeof(buff));
    buff[0] = POWER_SLAVE_FRAME_HEAD1;
    buff[1] = POWER_SLAVE_FRAME_HEAD2;
    buff[2] = POWER_SLAVE_CMD_CONTROL;
    buff[3] = fan_switch;
    buff[4] = duty_code;
    buff[5] = clear_flag;
    buff[6] = 0x00;
    buff[7] = POWER_SLAVE_FRAME_TAIL1;
    buff[8] = POWER_SLAVE_FRAME_TAIL2;

    uart_send_data(buff, POWER_SLAVE_CMD_LEN);
}

/* Power_ClearMaxMin：电源模块业务处理函数 */
void Power_ClearMaxMin(void)
{
    // 清零后由下一次DATA_Convert重新统计，避免上电0值长期作为最小值
    power_vin_u_max = 0;
    power_vin_u_min = 0xFFFF;
    power_12v_u_max = 0;
    power_12v_u_min = 0xFFFF;
    power_28v_u_max = 0;
    power_28v_u_min = 0xFFFF;
    power_f28v_u_max = 0;
    power_f28v_u_min = 0xFFFF;
    power_vin_i_max = 0;
    power_vin_i_min = 0xFFFF;
}

/* Power_SetWorkHour：电源模块业务处理函数 */
void Power_SetWorkHour(uint32_t hour)
{
    if (hour > 0xFFFFFFUL)
    {
        hour = 0xFFFFFFUL;
    }
    power_work_hour = hour;
    Power_EepromSetWorkHour(hour);
}

/* Power_GetWorkHour：电源模块业务处理函数 */
uint32_t Power_GetWorkHour(void)
{
    
    return power_work_hour;
}

/* Power_StatusReportTick：后台计时函数，默认只累计工作时长，不主动发送0x0412 */
void Power_StatusReportTick(uint16_t tick_ms)
{
    
    power_work_ms_cnt += tick_ms;

    while (power_work_ms_cnt >= 3600000UL)
    {
        power_work_ms_cnt -= 3600000UL;
        if (power_work_hour < 0xFFFFFFUL)
        {
            power_work_hour++;
            Power_EepromSetWorkHour(power_work_hour);
            Power_EepromSave();
        }
    }

#if POWER_SELF_CHECK_AUTO_REPORT_ENABLE
    /* 保留旧主动上报逻辑，但默认编译屏蔽。 */
    power_report_ms_cnt += tick_ms;
    if (power_report_ms_cnt >= POWER_STATUS_REPORT_PERIOD_MS)
    {
        power_report_ms_cnt = 0;
        Power_SendSelfCheck(power_self_check_type);
    }
#endif
}

/* Power_CheckVin270：电源模块业务处理函数 */
static uint8_t Power_CheckVin270(uint16_t vin)
{
    if (vin >= 25000 && vin <= 28000)
    {
        return 0x00;     // 正常
    }
    else if ((vin >= 24000 && vin < 25000) || (vin > 28000 && vin <= 29000))
    {
        return 0x03;     // 告警
    }
    else
    {
        return 0x01;     // 故障
    }
}

/* Power_CheckNominal：电源模块业务处理函数 */
static uint8_t Power_CheckNominal(uint16_t value, uint16_t nominal)
{
    uint32_t low_10 = (uint32_t)nominal * 90 / 100;
    uint32_t high_10 = (uint32_t)nominal * 110 / 100;
    uint32_t low_20 = (uint32_t)nominal * 80 / 100;
    uint32_t high_20 = (uint32_t)nominal * 120 / 100;

    if (value >= low_10 && value <= high_10)
    {
        return 0x00;
    }
    else if (value >= low_20 && value <= high_20)
    {
        return 0x03;
    }
    else
    {
        return 0x01;
    }
}

/* Power_GetFan28U：电源模块业务处理函数 */
static uint16_t Power_GetChannel28U(void)
{
    if (power_slave_online != 0)
    {
        /* 28PIN上传的F28VU作为主机对外0x0412中的“输出28V电压值（信道）”。；这样外部管理软件查询0x0412时，33~38字节全部使用28PIN实测28V及其最大/最小值。 */
        return power_slave_f28v_u;
    }

    /* 28PIN尚未上线时，为了上电初期仍有有效值，临时使用主机本地28V采样作为备用。 */
    return value_28V_U;
}

static uint16_t Power_GetFan28U(void)
{
    if (power_slave_online != 0)
    {
        /* 28PIN上传的F28VU同时作为整机风机28V电压字段来源。 */
        return power_slave_f28v_u;
    }

    return value_28V_U;
}

/* Power_MakeVoltageStatus：电源模块业务处理函数 */
static uint32_t Power_MakeVoltageStatus(void)
{
    
    uint8_t vin_status;
    
    uint8_t mid_status;
    
    uint8_t v12_status;
    
    uint8_t v28_status;
    
    uint8_t fan28_status;
    
    uint32_t status = 0;

    vin_status = Power_CheckVin270(value_VIN_U);
    mid_status = 0x00;
    v12_status = Power_CheckNominal(value_12V_U, 1200);
    v28_status = Power_CheckNominal(Power_GetChannel28U(), 2800);
    fan28_status = Power_CheckNominal(Power_GetFan28U(), 2800);

    status |= ((uint32_t)vin_status & 0x03) << 22;
    status |= ((uint32_t)mid_status & 0x03) << 20;
    status |= ((uint32_t)v12_status & 0x03) << 14;
    status |= ((uint32_t)v28_status & 0x03) << 12;
    status |= ((uint32_t)fan28_status & 0x03) << 10;

    if (vin_status == 0x01 || v12_status == 0x01 || v28_status == 0x01 || fan28_status == 0x01)
    {
        power_self_check_state = 0x01;
    }
    else if (vin_status == 0x03 || v12_status == 0x03 || v28_status == 0x03 || fan28_status == 0x03)
    {
        power_self_check_state = 0x02;
    }
    else
    {
        power_self_check_state = 0x00;
    }

    
    return status;
}

/* Power_SetFan：电源模块业务处理函数 */
/* Power_GetFaultOrWarnState：获取当前故障/告警状态，供LED控制和协议上报统一使用 */
uint8_t Power_GetFaultOrWarnState(void)
{
    if ((fatal_error_flag != 0) || (power_self_check_state == 0x01))
    {
        return 0x01;
    }

    if ((error_flag != 0) || (power_self_check_state == 0x02))
    {
        return 0x02;
    }

    return 0x00;
}

/* Power_IsFaultOrWarn：判断当前是否存在故障或告警，LED任务调用本函数，不再依赖事件组 */
uint8_t Power_IsFaultOrWarn(void)
{
    if (Power_GetFaultOrWarnState() != 0x00)
    {
        return 1;
    }

    return 0;
}

void Power_SetFan(uint8_t fan_switch, uint8_t duty_code)
{
    power_fan_switch = fan_switch;
    power_fan_duty = duty_code;

    if (fan_switch == POWER_FAN_ON)
    {
        EN_F28V_KZ_ON;
    }
    else
    {
        EN_F28V_KZ_OFF;
        power_fan_duty = 0x00;
    }

#if POWER_FAN_PWM_USE_TIM4
    TIM_SetCompare3(TIM4, ((uint16_t)(power_fan_duty >> 4) * POWER_PWM_PERIOD) / 10);
    TIM_SetCompare4(TIM4, ((uint16_t)(power_fan_duty >> 4) * POWER_PWM_PERIOD) / 10);
#endif
}

/* DATA_Convert：ADC采样滤波结果换算为协议使用的0.01V/0.01A等工程值 */
void DATA_Convert(void)
{
    compute_Voltage();
//    Weighted_Moving_Average(Average, Average_filter);

    wendu = Temp_transition_int(ADC_TEMP);
    value_TEMP = (uint16_t)(Power_EncodeTemperature(wendu));
//    value_12V_I = -5.317660675E-08f*ADC_12V_I*ADC_12V_I*ADC_12V_I + 5.852143557E-05f*ADC_12V_I*ADC_12V_I + 0.3978594518f*ADC_12V_I + 2.899096952f;
//    value_12V_U = 2.022636067E-08f*ADC_12V_U*ADC_12V_U*ADC_12V_U + -6.558812539E-05f*ADC_12V_U*ADC_12V_U + 0.6727864702f*ADC_12V_U + 1.06591119f;
//    value_28V_I = 1.338135391E-08f*ADC_28V_I*ADC_28V_I*ADC_28V_I + -3.677938085E-05f*ADC_28V_I*ADC_28V_I + 0.6425711879f*ADC_28V_I + -4.303419695f;
//    value_28V_U = 1.338135391E-08f*ADC_28V_U*ADC_28V_U*ADC_28V_U + -3.677938085E-05f*ADC_28V_U*ADC_28V_U + 0.6425711879f*ADC_28V_U + -4.303419695f;
//    value_VIN_I = 1.338135391E-08f*ADC_VIN_I*ADC_VIN_I*ADC_VIN_I + -3.677938085E-05f*ADC_VIN_I*ADC_VIN_I + 0.6425711879f*ADC_VIN_I + -4.303419695;
//    value_VIN_U = 1.338135391E-08f*ADC_VIN_U*ADC_VIN_U*ADC_VIN_U + -3.677938085E-05f*ADC_VIN_U*ADC_VIN_U + 0.6425711879f*ADC_VIN_U + -4.303419695f;

    Power_UpdateMaxMin(value_VIN_U, &power_vin_u_max, &power_vin_u_min);
    Power_UpdateMaxMin(value_12V_U, &power_12v_u_max, &power_12v_u_min);

    /* 28PIN在线后，0x0412的“输出28V电压值（信道）”必须使用28PIN上传F28VU，；最大/最小值也只由28PIN上传帧刷新，避免主机本地ADC值混入统计。；28PIN未上线时，保留主机本地28V采样作为备用统计。 */
    if (power_slave_online == 0)
    {
        Power_UpdateMaxMin(value_28V_U, &power_28v_u_max, &power_28v_u_min);
        Power_UpdateMaxMin(value_28V_U, &power_f28v_u_max, &power_f28v_u_min);
    }

    Power_UpdateMaxMin(value_VIN_I, &power_vin_i_max, &power_vin_i_min);

    Power_MakeVoltageStatus();
}

/* Power_SendAck：组包并发送0x0410 ACK/NAK应答帧 */
void Power_SendAck(uint16_t ack_msg_id, uint8_t ack_nak, uint8_t err_code, uint8_t set_flag)
{
    
    uint8_t buff[16];
    memset(buff, 0, sizeof(buff));

    buff[0] = POWER_TYPE_ACK;
    buff[1] = POWER_DEVICE_ID;
    uart_write_u16_be(&buff[2], POWER_MSG_ACK_MODULE);
    uart_write_u16_be(&buff[4], 16);

    uart_write_u16_be(&buff[6], ack_msg_id);
    buff[8] = ack_nak;
    buff[9] = err_code;
    buff[10] = set_flag;
    buff[11] = 0x00;
    buff[12] = 0x00;
    buff[13] = 0x00;

    uart_send_protocol_frame(POWER_EXT_UART_INDEX, buff, 14);
}

/* Power_SendSelfCheck：组包并发送0x0412电源模块自检应答 */
/* Power_SendSelfCheck：组包并发送0x0412电源模块自检应答，默认只由外部0x0401命令触发 */
void Power_SendSelfCheck(uint8_t self_check_type)
{
    
    uint8_t buff[88];
    
    uint32_t voltage_status;
    
    uint16_t fan28_u;
    
    uint16_t channel28_u;

    memset(buff, 0, sizeof(buff));

    voltage_status = Power_MakeVoltageStatus();
    channel28_u = Power_GetChannel28U();
    fan28_u = Power_GetFan28U();

    buff[0] = POWER_TYPE_ACK;
    buff[1] = POWER_DEVICE_ID;
    uart_write_u16_be(&buff[2], POWER_MSG_SELF_CHECK_ACK);
    uart_write_u16_be(&buff[4], 88);

    buff[6] = self_check_type;

    Power_EepromGetBoardId(&buff[7]);
    Power_EepromGetMfgDate(&buff[13]);

    // 软件版本号：V1.1
    buff[16] = 0x00;
    buff[17] = 0x11;

    // 板卡自检状态：0正常，1故障，2告警
    /* 板卡自检状态：由统一故障/告警标志生成，用于0x0412上报。 */
    buff[18] = Power_GetFaultOrWarnState();

    Power_FillU16(buff, 19, value_VIN_U);
    Power_FillU16(buff, 21, value_VIN_U);       // 中间转换电压，厂家提供值；当前无独立采样，暂用VIN
    Power_FillU16(buff, 23, power_vin_u_max);
    Power_FillU16(buff, 25, power_vin_u_min);
    Power_FillU16(buff, 27, value_12V_U);
    Power_FillU16(buff, 29, power_12v_u_max);
    Power_FillU16(buff, 31, power_12v_u_min);
    /* 输出28V电压值（信道）：使用28PIN上传F28VU；若28PIN尚未上传，则备用主机本地28V采样。；最大/最小值与当前值同源，28PIN在线后由Power_HandleSlaveFrame()刷新。 */
    Power_FillU16(buff, 33, channel28_u);
    Power_FillU16(buff, 35, power_28v_u_max);
    Power_FillU16(buff, 37, power_28v_u_min);
    Power_FillU16(buff, 39, fan28_u);
    Power_FillU16(buff, 41, power_f28v_u_max);
    Power_FillU16(buff, 43, power_f28v_u_min);

    buff[45] = (uint8_t)((voltage_status >> 16) & 0xFF);
    buff[46] = (uint8_t)((voltage_status >> 8) & 0xFF);
    buff[47] = (uint8_t)(voltage_status & 0xFF);
    buff[48] = 0x00;

    Power_FillU16(buff, 49, value_VIN_I);
    Power_FillU16(buff, 51, power_vin_i_max);
    Power_FillU16(buff, 53, power_vin_i_min);

    buff[73] = Power_EncodeTemperature(wendu);
    buff[74] = (uint8_t)((power_work_hour >> 16) & 0xFF);
    buff[75] = (uint8_t)((power_work_hour >> 8) & 0xFF);
    buff[76] = (uint8_t)(power_work_hour & 0xFF);

    // 风机状态：bit15~bit7表示风机1~9，0运转，1停转
    if (power_slave_online != 0)
    {
        // 从机上传帧的11~12字节携带RD检测位图
        buff[77] = (uint8_t)((power_slave_s0 >> 8) & 0xFF);
        buff[78] = (uint8_t)(power_slave_s0 & 0xFF);
    }
    else if ((power_fan_switch == POWER_FAN_ON) && (power_slave_fan_speed != 0))
    {
        // 从机未在线时的降级逻辑：有转速判定为全运转
        buff[77] = 0x00;
        buff[78] = 0x00;
    }
    else
    {
        // 从机未在线且无转速时，默认全停转
        buff[77] = 0xFF;
        buff[78] = 0x80;
    }

    uart_send_protocol_frame(POWER_EXT_UART_INDEX, buff, 86);
}

/* Power_SendStatusReport：电源模块业务处理函数 */
void Power_SendStatusReport(uint8_t self_check_type)
{
    Power_SendSelfCheck(self_check_type);
}

/* Power_SendUpdateDataAck：组包并发送0x041E软件升级数据包传输确认帧 */
void Power_SendUpdateDataAck(uint8_t confirm_type, uint8_t *name, uint8_t name_len, uint16_t pack_no)
{
    
    uint8_t buff[40];
    
    uint8_t i;
    memset(buff, 0, sizeof(buff));

    if (name_len > 24)
    {
        name_len = 24;
    }

    buff[0] = POWER_TYPE_UPDATE;
    buff[1] = POWER_DEVICE_ID;
    uart_write_u16_be(&buff[2], POWER_MSG_UPDATE_DATA_ACK);
    uart_write_u16_be(&buff[4], 40);

    buff[6] = 0x00;
    buff[7] = 0x00;
    buff[8] = confirm_type;
    buff[9] = name_len;

    if (name != 0)
    {
        for (i = 0; i < name_len; i++)
        {
            buff[10 + i] = name[i];
        }
    }

    Power_FillU16(buff, 34, pack_no);
    buff[36] = 0x00;
    buff[37] = 0x00;

    uart_send_protocol_frame(POWER_EXT_UART_INDEX, buff, 38);
}

/* Power_SaveHostAck：保存外部管理软件发来的0x0400内部ACK/NAK应答字段 */
void Power_SaveHostAck(uint16_t ack_msg_id, uint8_t ack_nak, uint8_t err_code, uint8_t set_flag)
{
    power_host_ack_msg_id = ack_msg_id;
    power_host_ack_nak = ack_nak;
    power_host_ack_err_code = err_code;
    power_host_ack_set_flag = set_flag;
    power_host_ack_valid = 1;
}

/* Power_SaveUpdateReportAck：保存外部管理软件发来的0x040F软件升级报告应答字段 */
void Power_SaveUpdateReportAck(uint8_t ack_value, uint8_t *name, uint8_t name_len, uint16_t report_crc)
{
    (void)name;

    if (name_len > 24)
    {
        name_len = 24;
    }

    power_update_report_ack_value = ack_value;
    power_update_report_ack_crc = report_crc;
    power_update_report_ack_valid = 1;
}

/* Power_SendUpdateReport：组包并发送0x041F软件升级报告帧，完善在线升级报告发送框架 */
void Power_SendUpdateReport(uint8_t report_state, uint8_t *name, uint8_t name_len, uint16_t program_crc)
{
    uint8_t buff[40];
    uint8_t i;

    memset(buff, 0, sizeof(buff));

    if (name_len > 24)
    {
        name_len = 24;
    }

    buff[0] = POWER_TYPE_UPDATE;
    buff[1] = POWER_DEVICE_ID;
    uart_write_u16_be(&buff[2], POWER_MSG_UPDATE_REPORT);
    uart_write_u16_be(&buff[4], 40);

    buff[6] = 0x00;
    buff[7] = 0x00;
    buff[8] = report_state;
    buff[9] = name_len;

    if (name != 0)
    {
        for (i = 0; i < name_len; i++)
        {
            buff[10 + i] = name[i];
        }
    }

    Power_FillU16(buff, 34, program_crc);
    buff[36] = 0x00;
    buff[37] = 0x00;

    uart_send_protocol_frame(POWER_EXT_UART_INDEX, buff, 38);
}

void test(void)
{
//    RCC_ClocksTypeDef RCC_CLK;
//    RCC_GetClocksFreq(&RCC_CLK);

    printf("\n-----------------------------------------------\n");
		printf("value_TEMP =%d \n", ADC_TEMP);
    printf("温度 =%d \n", wendu);
    printf("value_12V_I %.2f [%d]\n", value_12V_I / 100.0, ADC_12V_I);
    printf("value_12V_U %.2f [%d]\n", value_12V_U / 100.0, ADC_12V_U);
    printf("value_28V_I %.2f [%d]\n", value_28V_I / 100.0, ADC_28V_I);
    printf("value_28V_U %.2f [%d]\n", value_28V_U / 100.0, ADC_28V_U);
    printf("value_VIN_I %.2f [%d]\n", value_VIN_I / 100.0, ADC_VIN_I);
    printf("value_VIN_U %.2f [%d]\n", value_VIN_U / 100.0, ADC_VIN_U);
    printf("fan_switch =%d duty=0x%02X\n", power_fan_switch, power_fan_duty);
    printf("slave_f28v=%d fan_speed=%d i_f1=%d y=%d f_pwm=%d online=%d\n", power_slave_f28v_u, power_slave_fan_speed, power_slave_i_f1, power_slave_y, power_slave_f_pwm, power_slave_online);

//    printf("-----------------------------------------------\n");
//    printf("当前系统主频: %dMHz\r\n", SystemCoreClock / 1000000);
//    printf("SYSCLK      : %dMHz\r\n", RCC_CLK.SYSCLK_Frequency / 1000000);
//    printf("HCLK        : %dMHz\r\n", RCC_CLK.HCLK_Frequency / 1000000);
//    printf("PCLK1       : %dMHz\r\n", RCC_CLK.PCLK1_Frequency / 1000000);
//    printf("PCLK2       : %dMHz\r\n", RCC_CLK.PCLK2_Frequency / 1000000);
}
