#ifndef __POWER_H

#define __POWER_H

#include "sys.h"
#include <math.h>
#include "delay.h"
#include "usart.h"
#include "GPIO.h"
#include "nvic.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* -------------------- 对外ICD协议消息ID -------------------- */

#define POWER_MSG_ACK_HOST          0x0400
#define POWER_MSG_SELF_CHECK_REQ    0x0401
#define POWER_MSG_CONTROL           0x0403
#define POWER_MSG_BOARD_CFG         0x0404
#define POWER_MSG_UPDATE_DATA       0x040E
#define POWER_MSG_UPDATE_REPORT_ACK 0x040F
#define POWER_MSG_ACK_MODULE        0x0410
#define POWER_MSG_SELF_CHECK_ACK    0x0412
#define POWER_MSG_UPDATE_DATA_ACK   0x041E
#define POWER_MSG_UPDATE_REPORT     0x041F

/* -------------------- 对外ICD协议消息类型 -------------------- */

#define POWER_TYPE_SET              0xF0
#define POWER_TYPE_REQ              0xF1
#define POWER_TYPE_ACK              0xF2
#define POWER_TYPE_REPORT           0xF3
#define POWER_TYPE_UPDATE           0xFA
#define POWER_DEVICE_ID             0x00
#define POWER_ACK                   0x00
#define POWER_NAK                   0x01
#define POWER_SET_OK                0x00
#define POWER_SET_FAIL              0x01
#define POWER_SELF_POWERON          0x01
#define POWER_SELF_PERIOD           0x02
#define POWER_SELF_MAINTAIN         0x03
#define POWER_SELF_INIT             0x04
#define POWER_FAN_OFF               0x00
#define POWER_FAN_ON                0x01
#define POWER_UPDATE_PACKET_START   0x01
#define POWER_UPDATE_PACKET_CONT    0x02
#define POWER_UPDATE_PACKET_END     0x03
#define POWER_UPDATE_CONFIRM_OK     0x01
#define POWER_UPDATE_CONFIRM_CRCERR 0x02
#define POWER_UPDATE_CONFIRM_RESEND 0x03
#define POWER_UPDATE_REPORT_SUCCESS 0x01
#define POWER_UPDATE_REPORT_FAIL    0x02
#define POWER_UPDATE_REPORT_CRC     0x03
#define POWER_UPDATE_REPORT_ACK_SUCCESS 0x01
#define POWER_UPDATE_REPORT_ACK_FAIL    0x02
#define POWER_UPDATE_REPORT_ACK_CRC_OK  0x03
#define POWER_UPDATE_REPORT_ACK_CRC_ERR 0x04
#define POWER_UPDATE_REPORT_AUTO_SEND_ENABLE 0
#define POWER_FAN_PWM_USE_TIM4      0       /* 若硬件确认风机PWM接TIM4_CH3/CH4，可改为1 */
/* PWM周期上限，与从机pwm_config(899,7)对应，10kHz：72MHz/8/900=10000Hz */
#define POWER_PWM_PERIOD            899

/* 风机自动温度控制策略 */
#define POWER_FAN_TEMP_THRESH_HIGH  55    /* 高温阈值(°C)，高于此值全速运行 */
#define POWER_FAN_TEMP_THRESH_MID   40    /* 中温阈值(°C)，高于此值中速运行 */
#define POWER_FAN_SPEED_HIGH        0xA0  /* 高速占空比（100%） */
#define POWER_FAN_SPEED_MID         0x60  /* 中速占空比（60%） */
#define POWER_FAN_SPEED_LOW         0x30  /* 低速占空比（30%） */
#define POWER_FAN_STARTUP_SEC       120U  /* 上电高速持续时间（秒），2分钟 */
#define POWER_FAN_CMD_KEEP_SEC      120U  /* 外部命令控制保持时间（秒），2分钟后恢复温度控制 */

/*表示是不是主动回传0412*/
#define POWER_SELF_CHECK_AUTO_REPORT_ENABLE 0

/* 后台计时周期，只用于累计工作时长；默认不触发串口主动上报。 */

#define POWER_STATUS_REPORT_PERIOD_MS 1000
#define POWER_SLAVE_FRAME_HEAD1     0x55
#define POWER_SLAVE_FRAME_HEAD2     0xAA
#define POWER_SLAVE_FRAME_TAIL1     0x66
#define POWER_SLAVE_FRAME_TAIL2     0xBB
#define POWER_SLAVE_UPLOAD_LEN      21
#define POWER_SLAVE_CMD_LEN         9
#define POWER_SLAVE_CMD_CONTROL     0xC1
extern uint16_t power_slave_f28v_u;
extern uint16_t power_slave_fan_speed;
extern uint16_t power_slave_i_f1;
extern uint8_t power_slave_y;
extern uint16_t power_slave_f_pwm;
extern uint16_t power_slave_s0;
extern uint16_t power_slave_s1;
extern uint16_t power_slave_s2;
extern uint16_t power_slave_temp;
extern uint8_t power_slave_online;

/* -------------------- 0x0400 / 0x040F / 0x041F框架状态缓存 -------------------- */

extern uint8_t power_host_ack_valid;
extern uint16_t power_host_ack_msg_id;
extern uint8_t power_host_ack_nak;
extern uint8_t power_host_ack_err_code;
extern uint8_t power_host_ack_set_flag;
extern uint8_t power_update_report_ack_valid;
extern uint8_t power_update_report_ack_value;
extern uint16_t power_update_report_ack_crc;


extern uint16_t ADC_results[M];

// ADC_results 数组宏定义（用于ADC DMA采集结果）

#define value_TEMP      ADC_results[0]   // PB1 - 温度采集
#define value_12V_I     ADC_results[1]   // PB0 - 12V输出电流
#define value_12V_U     ADC_results[2]   // PA7 - 12V输出电压
#define value_28V_I     ADC_results[3]   // PA6 - 28V输出电流
#define value_28V_U     ADC_results[4]   // PA5 - 28V输出电压
#define value_VIN_I     ADC_results[5]   // PA4 - 输入电流
#define value_VIN_U     ADC_results[6]   // PA0 - 输入电压

// Average_filter 数组宏定义（用于滤波后的结果）

#define ADC_TEMP        Average_filter[0]
#define ADC_12V_I       Average_filter[1]
#define ADC_12V_U       Average_filter[2]
#define ADC_28V_I       Average_filter[3]
#define ADC_28V_U       Average_filter[4]
#define ADC_VIN_I       Average_filter[5]
#define ADC_VIN_U       Average_filter[6]

extern float vin_filtered;
extern float iout_filtered;
extern float vout_filtered;
extern uint8_t vo1_shutdown_flag;
extern uint8_t vo2_shutdown_flag;
extern uint8_t power_fan_switch;
extern uint8_t power_fan_duty;
extern uint8_t power_self_check_type;
extern uint8_t power_self_check_state;
extern uint16_t fatal_error_flag;
extern uint8_t error_flag;

/* -------------------- 故障/告警状态值 -------------------- */
/* 0x0412板卡自检状态：正常。 */
#define POWER_SELF_STATE_NORMAL    0x00
/* 0x0412板卡自检状态：故障。 */
#define POWER_SELF_STATE_FAULT     0x01
/* 0x0412板卡自检状态：告警。 */
#define POWER_SELF_STATE_WARN      0x02

typedef struct
{
    
    uint8_t OV_FLAG;
    
    uint8_t UV_FLAG;
    
    uint16_t Restore_cycle;
    
    uint16_t Filtering_cycle;
} FAULT_Assemble;

/* 函数声明说明：test，test函数，保持原工程接口并完成对应模块处理。 */
void test(void);
/* 函数声明说明：DATA_Convert，ADC采样滤波结果换算为协议使用的0.01V/0.01A等工程值。 */
void DATA_Convert(void);

void Power_ClearMaxMin(void);
void Power_SetWorkHour(uint32_t hour);
uint32_t Power_GetWorkHour(void);
/* 函数声明说明：Power_StatusReportTick，后台计时函数，默认只累计工作时长，不主动发送0x0412。 */
void Power_StatusReportTick(uint16_t tick_ms);
void Power_SendStatusReport(uint8_t self_check_type);
void Power_SetFan(uint8_t fan_switch, uint8_t duty_code);
/* 函数声明说明：Power_SendAck，组包并发送0x0410 ACK/NAK应答帧。 */
void Power_SendAck(uint16_t ack_msg_id, uint8_t ack_nak, uint8_t err_code, uint8_t set_flag);
/* 函数声明说明：Power_SendSelfCheck，组包并发送0x0412电源模块自检应答，默认只由外部0x0401命令触发。 */
void Power_SendSelfCheck(uint8_t self_check_type);
/* 函数声明说明：Power_SendUpdateDataAck，组包并发送0x041E软件升级数据包传输确认帧。 */
void Power_SendUpdateDataAck(uint8_t confirm_type, uint8_t *name, uint8_t name_len, uint16_t pack_no);
/* 函数声明说明：Power_SaveHostAck，保存0x0400外部管理软件ACK/NAK应答内容，完善主动消息确认框架。 */
void Power_SaveHostAck(uint16_t ack_msg_id, uint8_t ack_nak, uint8_t err_code, uint8_t set_flag);
/* 函数声明说明：Power_SaveUpdateReportAck，保存0x040F软件升级报告应答内容，完善在线升级报告闭环框架。 */
void Power_SaveUpdateReportAck(uint8_t ack_value, uint8_t *name, uint8_t name_len, uint16_t report_crc);
/* 函数声明说明：Power_SendUpdateReport，组包并发送0x041F软件升级报告帧，供Bootloader或升级状态机调用。 */
void Power_SendUpdateReport(uint8_t report_state, uint8_t *name, uint8_t name_len, uint16_t program_crc);
uint8_t Power_GetFaultOrWarnState(void);
uint8_t Power_IsFaultOrWarn(void);
/* 根据value_VIN_U更新SRDD引脚电平：<210V输出高，>=210V输出低 */
void Power_UpdateSRDD(void);
/* 风机自动控制Tick，每秒调用一次；管理上电高速→温度控制→命令控制状态机 */
void Power_FanAutoControlTick(void);
/* 外部0x0403命令到来时调用，进入命令控制模式，2分钟后自动恢复温度控制 */
void Power_FanEnterCmdMode(void);
/* 函数声明说明：Power_ProcessSlaveUpload，从PA2/PA3内部串口环形缓冲取出字节，解析28PIN上传的55 AA ... 66 BB采集帧。 */
void Power_ProcessSlaveUpload(void);
/* 函数声明说明：Power_SendSlaveControl，48PIN收到外部0x0403控制命令后，通过PA2/PA3向28PIN下发55 AA C1 ... 66 BB控制帧。 */
void Power_SendSlaveControl(uint8_t fan_switch, uint8_t duty_code, uint8_t clear_flag);

#endif
