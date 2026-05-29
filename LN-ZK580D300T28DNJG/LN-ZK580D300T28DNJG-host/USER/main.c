/*
**----------------------------------文件信息------------------------------------
** 文件名称: main.c
** 创建人员: 胡兴明
** 修改说明: 48PIN电源模块主程序，PB10/PB11对外ICD通信，PA2/PA3与28PIN内部通信
**------------------------------------------------------------------------------
*/

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <math.h>
#include <string.h>
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "GPIO.h"
#include "nvic.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "power.h"
#include "power_eeprom.h"

/*-------------------- 版本号 --------------------*/
#define Mversion 1
#define Sversion 1
#define uart_debug 0

/*-------------------- 任务优先级 --------------------*/
#define START_TASK_PRIO         1
#define LED_TASK_PRIO           2
#define STATUS_REPORT_TASK_PRIO 3
#define ADC_TASK_PRIO           4
#define USART_TASK_PRIO         6
#define TEST_TASK_PRIO          2

/*-------------------- 任务堆栈大小 --------------------*/
#define START_STK_SIZE          256
#define LED_STK_SIZE            128
#define STATUS_REPORT_STK_SIZE  160
#define ADC_STK_SIZE            150
#define USART_STK_SIZE          256

#define TEST_STK_SIZE           350



/*-------------------- 全局句柄 --------------------*/
#if uart_debug
TaskHandle_t TESTTask_Handler;
#endif
TaskHandle_t StartTask_Handler;
TaskHandle_t ADCTask_Handler;

TaskHandle_t UARTTask_Handler;

TaskHandle_t LEDTask_Handler;
TaskHandle_t StatusReportTask_Handler;

xSemaphoreHandle FillBuffMutex = NULL;

xSemaphoreHandle SemaphoerCapOVPHandle = NULL;
QueueHandle_t Queuehandler_USART = NULL;
QueueHandle_t Queuehandler_ModOutNum = NULL;

queue_data Queue_USART;
int16_t wendu = 0;

/*-------------------- 函数声明 --------------------*/
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
void SYS_Init(void);
void start_task(void *pvParameters);
void ADC_task(void *pvParameters);
void USART_task(void *pvParameters);
void StatusReport_task(void *pvParameters);
void LED_task(void *pvParameters);
#if uart_debug
void test_task(void *pvParameters);
#endif

/*-------------------- 协议处理函数 --------------------*/
/*******************************************************************************
** 函数名称: Power_CheckFrameLen
** 功能描述: 电源模块业务处理函数。
** 参数说明: 
********************************************************************************/
static uint8_t Power_CheckFrameLen(uint8_t *frame, uint16_t frame_len)
{
    uint16_t len;

    if (frame_len < 6)
    {
        return UART_PROTOCOL_ERR_LEN;
    }

    len = uart_read_u16_be(&frame[4]);
    if (len != (frame_len + 2))
    {
        return UART_PROTOCOL_ERR_LEN;
    }


    return UART_PROTOCOL_OK;
}

/*******************************************************************************
** 函数名称: Power_IsValidDuty
** 功能描述: 电源模块业务处理函数。
** 参数说明:
********************************************************************************/
static uint8_t Power_IsValidDuty(uint8_t duty)
{
    if (duty <= 0xA0 && ((duty & 0x0F) == 0x00))
    {
        return 1;
    }
    return 0;
}

/*******************************************************************************
** 函数名称: Power_SendProtocolErrorByRaw
** 功能描述: 电源模块业务处理函数。
** 参数说明: 
********************************************************************************/
static void Power_SendProtocolErrorByRaw(uint8_t *raw_frame, uint16_t raw_len, uint8_t err_code)
{
    uint16_t msg_id;
    uint8_t name_len;

    uint16_t pack_no;

    /* CRC错误时严格丢弃整帧，不允许触发任何业务回传 */
    if (err_code == UART_PROTOCOL_ERR_CRC)
    {
        return;
    }

    if (raw_frame == 0 || raw_len < 6)
    {
        return;
    }

    msg_id = uart_read_u16_be(&raw_frame[2]);

    if (raw_frame[0] == POWER_TYPE_SET)
    {
        Power_SendAck(msg_id, POWER_NAK, err_code, POWER_SET_FAIL);
    }
    else if (raw_frame[0] == POWER_TYPE_UPDATE && msg_id == POWER_MSG_UPDATE_DATA && raw_len >= 36)
    {
        name_len = raw_frame[9];
        if (name_len > 24)
        {
            name_len = 0;
        }
        pack_no = uart_read_u16_be(&raw_frame[34]);
        Power_SendUpdateDataAck(POWER_UPDATE_CONFIRM_CRCERR, &raw_frame[10], name_len, pack_no);
    }
}

/*******************************************************************************
** 函数名称: Power_HandleSelfCheck
** 功能描述: 电源模块业务处理函数。
** 参数说明:
********************************************************************************/
static void Power_HandleSelfCheck(uint8_t *frame, uint16_t frame_len)
{
    uint8_t cmd;
    uint8_t type;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK)
    {
        return;
    }

    if (frame_len != 14 || frame[0] != POWER_TYPE_REQ)
    {
        return;
    }

    cmd = frame[6];
    type = frame[7];
    if (type < POWER_SELF_POWERON || type > POWER_SELF_INIT)
    {
        type = POWER_SELF_POWERON;
    }

    if (cmd < 0x01 || cmd > 0x03)
    {
        return;
    }

    power_self_check_type = type;


    if (cmd == 0x01)
    {
        Power_SendStatusReport(type);
    }
    else if (cmd == 0x02)
    {
        power_self_check_state = 0x00;
        Power_SendStatusReport(type);
    }
    else if (cmd == 0x03)
    {
        Power_SendStatusReport(type);
    }
}

/*******************************************************************************
** 函数名称: Power_HandleControl
** 功能描述: 电源模块业务处理函数。
** 参数说明: 
********************************************************************************/
static void Power_HandleControl(uint8_t *frame, uint16_t frame_len)
{

    uint8_t fan_switch;

    uint8_t duty;

    uint8_t clear_flag;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK || frame_len != 14)
    {
        Power_SendAck(POWER_MSG_CONTROL, POWER_NAK, UART_PROTOCOL_ERR_LEN, POWER_SET_FAIL);
        return;
    }

    if (frame[0] != POWER_TYPE_SET || frame[1] != POWER_DEVICE_ID)
    {
        Power_SendAck(POWER_MSG_CONTROL, POWER_NAK, UART_PROTOCOL_ERR_DEVICE, POWER_SET_FAIL);
        return;
    }

    fan_switch = frame[6];
    duty = frame[7];
    clear_flag = frame[8];

    if ((fan_switch != POWER_FAN_OFF && fan_switch != POWER_FAN_ON) ||
        Power_IsValidDuty(duty) == 0 ||
        (clear_flag > POWER_CLEAR_MAXMIN_AND_HOUR))
    {
        Power_SendAck(POWER_MSG_CONTROL, POWER_NAK, UART_PROTOCOL_ERR_DATA, POWER_SET_FAIL);
        return;
    }

    Power_SetFan(fan_switch, duty);
    Power_SendSlaveControl(fan_switch, duty, clear_flag);
    if (clear_flag == POWER_CLEAR_MAXMIN || clear_flag == POWER_CLEAR_MAXMIN_AND_HOUR)
    {
        Power_ClearMaxMin();
    }
    if (clear_flag == POWER_CLEAR_WORK_HOUR || clear_flag == POWER_CLEAR_MAXMIN_AND_HOUR)
    {
        Power_SetWorkHour(0);
        Power_EepromSave();
    }

    Power_SendAck(POWER_MSG_CONTROL, POWER_ACK, 0x00, POWER_SET_OK);
}

static void Power_HandleBoardCfg(uint8_t *frame, uint16_t frame_len)
{
    uint8_t sub_cmd;
    uint8_t ok = 0;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK || frame_len != 14)
    {
        Power_SendAck(POWER_MSG_BOARD_CFG, POWER_NAK, UART_PROTOCOL_ERR_LEN, POWER_SET_FAIL);
        return;
    }

    if (frame[0] != POWER_TYPE_SET || frame[1] != POWER_DEVICE_ID)
    {
        Power_SendAck(POWER_MSG_BOARD_CFG, POWER_NAK, UART_PROTOCOL_ERR_DEVICE, POWER_SET_FAIL);
        return;
    }

    sub_cmd = frame[6];
    switch (sub_cmd)
    {
    case POWER_EEPROM_CMD_SET_WORK_HOUR:
        ok = Power_EepromHandleSet(sub_cmd, &frame[7], 3);
        if (ok != 0)
        {
            Power_SetWorkHour(Power_EepromGetWorkHour());
        }
        break;
    case POWER_EEPROM_CMD_SET_BOARD_ID:
        ok = Power_EepromHandleSet(sub_cmd, &frame[7], 6);
        break;
    case POWER_EEPROM_CMD_SET_MFG_DATE:
        ok = Power_EepromHandleSet(sub_cmd, &frame[7], 3);
        break;
    case POWER_EEPROM_CMD_CLEAR_WORK_HOUR:
    case POWER_EEPROM_CMD_CLEAR_BOARD_ID:
    case POWER_EEPROM_CMD_CLEAR_MFG_DATE:
    case POWER_EEPROM_CMD_CLEAR_ALL:
        ok = Power_EepromHandleClear(sub_cmd);
        if (ok != 0)
        {
            Power_SetWorkHour(Power_EepromGetWorkHour());
        }
        break;
    default:
        ok = 0;
        break;
    }

    if (ok != 0)
    {
        Power_SendAck(POWER_MSG_BOARD_CFG, POWER_ACK, 0x00, POWER_SET_OK);
    }
    else
    {
        Power_SendAck(POWER_MSG_BOARD_CFG, POWER_NAK, UART_PROTOCOL_ERR_DATA, POWER_SET_FAIL);
    }
}

/*******************************************************************************
** 函数名称: Power_HandleUpdateData
** 功能描述: 电源模块业务处理函数。
** 参数说明:
********************************************************************************/
static void Power_HandleUpdateData(uint8_t *frame, uint16_t frame_len)
{
    uint8_t packet_type;
    uint8_t name_len;
    uint16_t pack_no;
    uint16_t text_len;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK || frame_len != (UART_UPDATE_FRAME_TOTAL_LEN - 2))
    {
        return;
    }

    if (frame[0] != POWER_TYPE_UPDATE || frame[1] != POWER_DEVICE_ID)
    {
        return;
    }

    packet_type = frame[8];
    name_len = frame[9];
    pack_no = uart_read_u16_be(&frame[34]);
    text_len = uart_read_u16_be(&frame[36]);

    if ((packet_type < POWER_UPDATE_PACKET_START || packet_type > POWER_UPDATE_PACKET_END) ||
        name_len > 24 || text_len > 1024)
    {
        Power_SendUpdateDataAck(POWER_UPDATE_CONFIRM_CRCERR, &frame[10], 0, pack_no);
        return;
    }

    /* 当前只完成协议握手，Flash擦写/回退需要Bootloader支持 */
    Power_SendUpdateDataAck(POWER_UPDATE_CONFIRM_OK, &frame[10], name_len, pack_no);

#if POWER_UPDATE_REPORT_AUTO_SEND_ENABLE
    /*
     * 0x041F自动发送框架：
     * 收到数据结束包后，可向上位机反馈待升级总程序CRC结果。
     * 当前应用层未接Bootloader完整CRC计算，默认宏关闭；接入后可填入真实CRC。
     */
    if (packet_type == POWER_UPDATE_PACKET_END)
    {
        Power_SendUpdateReport(POWER_UPDATE_REPORT_CRC, &frame[10], name_len, 0x0000);
    }
#endif
}

static void Power_HandleHostAck(uint8_t *frame, uint16_t frame_len)
{
    uint16_t ack_msg_id;
    uint8_t ack_nak;
    uint8_t err_code;
    uint8_t set_flag;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK || frame_len != 14)
    {
        return;
    }

    if (frame[0] != POWER_TYPE_ACK || frame[1] != POWER_DEVICE_ID)
    {
        return;
    }

    ack_msg_id = uart_read_u16_be(&frame[6]);
    ack_nak = frame[8];
    err_code = frame[9];
    set_flag = frame[10];

    if ((ack_nak > POWER_NAK) || (err_code > UART_PROTOCOL_ERR_DEVICE) || (set_flag > POWER_SET_FAIL))
    {
        return;
    }

    Power_SaveHostAck(ack_msg_id, ack_nak, err_code, set_flag);
}

/*******************************************************************************
** 函数名称: Power_HandleUpdateReportAck
** 功能描述: 解析0x040F外部管理软件到电源模块的软件升级报告应答。
** 参数说明: frame=不含SLIP和CRC的协议帧，frame_len=帧头+消息体长度。
** 返回说明: 无。
********************************************************************************/
static void Power_HandleUpdateReportAck(uint8_t *frame, uint16_t frame_len)
{
    uint8_t ack_value;
    uint8_t name_len;
    uint16_t report_crc;

    if (Power_CheckFrameLen(frame, frame_len) != UART_PROTOCOL_OK || frame_len != 38)
    {
        return;
    }

    if (frame[0] != POWER_TYPE_UPDATE || frame[1] != POWER_DEVICE_ID)
    {
        return;
    }

    ack_value = frame[8];
    name_len = frame[9];
    report_crc = uart_read_u16_be(&frame[34]);

    if ((ack_value < POWER_UPDATE_REPORT_ACK_SUCCESS) ||
        (ack_value > POWER_UPDATE_REPORT_ACK_CRC_ERR) ||
        (name_len > 24))
    {
        return;
    }

    Power_SaveUpdateReportAck(ack_value, &frame[10], name_len, report_crc);
}

/*******************************************************************************
** 函数名称: Power_HandleProtocolFrame
** 功能描述: 电源模块业务处理函数。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
static void Power_HandleProtocolFrame(uint8_t *frame, uint16_t frame_len)
{
    uint16_t msg_id;

    if (frame_len < 6)
    {
        return;
    }

    msg_id = uart_read_u16_be(&frame[2]);

    if (frame[1] != POWER_DEVICE_ID)
    {
        if (frame[0] == POWER_TYPE_SET)
        {
            Power_SendAck(msg_id, POWER_NAK, UART_PROTOCOL_ERR_DEVICE, POWER_SET_FAIL);
        }
        return;
    }

    switch (msg_id)
    {
    case POWER_MSG_ACK_HOST:
        Power_HandleHostAck(frame, frame_len);
        break;

    case POWER_MSG_SELF_CHECK_REQ:
        Power_HandleSelfCheck(frame, frame_len);
        break;

    case POWER_MSG_CONTROL:
        Power_HandleControl(frame, frame_len);
        break;

    case POWER_MSG_BOARD_CFG:
        Power_HandleBoardCfg(frame, frame_len);
        break;

    case POWER_MSG_UPDATE_DATA:
        Power_HandleUpdateData(frame, frame_len);
        break;

    case POWER_MSG_UPDATE_REPORT_ACK:
        Power_HandleUpdateReportAck(frame, frame_len);
        break;

    default:
        if (frame[0] == POWER_TYPE_SET)
        {
            Power_SendAck(msg_id, POWER_NAK, UART_PROTOCOL_ERR_ID, POWER_SET_FAIL);
        }
        break;
    }
}

/*******************************************************************************
** 函数名称: main
** 功能描述: 只做三件事：建队列/信号量，初始化硬件，创建start_task
********************************************************************************/
int main(void)
{
    Queuehandler_USART = xQueueCreate(1, sizeof(Queue_USART));
    Queuehandler_ModOutNum = xQueueCreate(1, 1);
    FillBuffMutex = xSemaphoreCreateMutex();
    SemaphoerCapOVPHandle = xSemaphoreCreateBinary();

    SYS_Init();

    xTaskCreate((TaskFunction_t)start_task,
                (const char *)"start_task",
                (uint16_t)START_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)START_TASK_PRIO,
                (TaskHandle_t *)&StartTask_Handler);

    vTaskStartScheduler();

    while (1)
    {
    }
}

/*******************************************************************************
** 函数名称: start_task
** 功能描述: 统一创建业务任务，创建完成后删除自己
********************************************************************************/
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();

    xTaskCreate((TaskFunction_t)ADC_task,
                (const char *)"ADC_task",
                (uint16_t)ADC_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)ADC_TASK_PRIO,
                (TaskHandle_t *)&ADCTask_Handler);

    xTaskCreate((TaskFunction_t)USART_task,
                (const char *)"USART_task",
                (uint16_t)USART_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)USART_TASK_PRIO,
                (TaskHandle_t *)&UARTTask_Handler);

    xTaskCreate((TaskFunction_t)StatusReport_task,
                (const char *)"Status_task",
                (uint16_t)STATUS_REPORT_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)STATUS_REPORT_TASK_PRIO,
                (TaskHandle_t *)&StatusReportTask_Handler);

    xTaskCreate((TaskFunction_t)LED_task,
                (const char *)"LED_task",
                (uint16_t)LED_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)LED_TASK_PRIO,
                (TaskHandle_t *)&LEDTask_Handler);

#if uart_debug
    xTaskCreate((TaskFunction_t)test_task,
                (const char *)"TEST_task",
                (uint16_t)TEST_STK_SIZE,
                (void *)NULL,
                (UBaseType_t)TEST_TASK_PRIO,
                (TaskHandle_t *)&TESTTask_Handler);
#endif

    taskEXIT_CRITICAL();
    vTaskDelete(NULL);
}

/*******************************************************************************
** 函数名称: ADC_task
** 功能描述: 只负责ADC采样滤波和数据换算，不负责串口主动上传
********************************************************************************/
void ADC_task(void *pvParameters)
{
    while (1)
    {
        DATA_Convert();
        vTaskDelay(20);
    }
}

/*******************************************************************************
** 函数名称: StatusReport_task
** 功能描述: 取消主动上传0x0412状态帧；只保留后台计时
********************************************************************************/

void StatusReport_task(void *pvParameters)
{

#if POWER_SELF_CHECK_AUTO_REPORT_ENABLE
    vTaskDelay(500);
    Power_SendStatusReport(power_self_check_type);
#endif

    while (1)
    {
        vTaskDelay(POWER_STATUS_REPORT_PERIOD_MS);
        Power_StatusReportTick(POWER_STATUS_REPORT_PERIOD_MS);
    }
}

/*******************************************************************************
** 函数名称: USART_task
** 功能描述: 接收PB10/PB11外部ICD帧，同时轮询PA2/PA3从机采集帧
********************************************************************************/

void USART_task(void *pvParameters)
{
    static uint8_t raw_buff[UART_FRAME_MAX_LEN];
    static uint8_t frame_buff[UART_FRAME_MAX_LEN];
    uint16_t raw_len;
    uint16_t frame_len;

    uint8_t ret;

    while (1)
    {
        Power_ProcessSlaveUpload();
        raw_len = 0;
        if (uart2_get_rx_frame(raw_buff, &raw_len) == 0)
        {
            vTaskDelay(2);
            continue;
        }

        frame_len = 0;
        ret = uart_protocol_unpack(raw_buff, raw_len, frame_buff, &frame_len);
        if (ret != UART_PROTOCOL_OK)
        {
            /* CRC错误帧必须严格丢弃，不能回ACK/NAK，也不能回0x041E */
            if (ret != UART_PROTOCOL_ERR_CRC)
            {
                Power_SendProtocolErrorByRaw(raw_buff, raw_len, ret);
            }
            continue;
        }

        Power_HandleProtocolFrame(frame_buff, frame_len);
    }
}

/*******************************************************************************
** 函数名称: LED_task
** 功能描述: 正常亮绿灯，故障/告警亮红灯
********************************************************************************/

void LED_task(void *pvParameters)
{
    while (1)
    {
        if (Power_IsFaultOrWarn() != 0)
        {
            LED_G1_OFF;
            LED_G2_OFF;
            LED_R1_ON;
            LED_R2_ON;
        }
        else
        {
            LED_R1_OFF;
            LED_R2_OFF;
            LED_G1_ON;
            LED_G2_ON;
        }

        vTaskDelay(200);
    }
}

#if uart_debug
/*******************************************************************************
** 函数名称: test_task
** 功能描述: test_task函数，保持原工程接口并完成对应模块处理。
** 参数说明: 
********************************************************************************/
void test_task(void *pvParameters)
{
    char strbuf1[700];

    while (1)
    {
        test();
        printf("\r\n");
        vTaskGetRunTimeStats(strbuf1);
        printf("任务           时间             占用");
        printf("\n%s\n", strbuf1);
        vTaskList(strbuf1);
        printf("-----------------------------------------------\n");
        printf("任务        当前状态 优先级 最小剩余 创建顺序");
        printf("\n%s\n", strbuf1);
        vTaskDelay(600);
    }
}
#endif

/*******************************************************************************
** 函数名称: fputc
** 功能描述: 
********************************************************************************/
int fputc(int ch, FILE *f)
{
    Usart_SendByte(DEBUG_USARTx, (uint8_t)ch);
    return ch;
}

/*******************************************************************************
** 函数名称: fgetc
** 功能描述:
********************************************************************************/
int fgetc(FILE *f)
{
    while (usart_flag_get(DEBUG_USARTx, USART_FLAG_RBNE) == RESET)
        ;
    return (int)usart_data_receive(DEBUG_USARTx);
}


/*******************************************************************************
** 函数名称: SYS_Init
** 功能描述: 主机系统初始化入口。

********************************************************************************/
void SYS_Init(void)
{
    GPIO_init_all();
    delay_init();
    MYDMA_Config();
     adc_config(); 
    TIM2_init_all(499, 1199);
//    Filter_Init();
    uart_init(115200);    // PA2/PA3，USART1，两个单片机之间通信
    uart2_init(115200);   // PB10/PB11，USART2，对外管理协议通信
    NVIC_init_all();
    delay_ms(50);

    EN_12V_KZ_ON;
    EN_28V_KZ_ON;
    EN_F28V_KZ_ON;
		G_PWM_ON;
		VOPEN_ON;
		GZ_ON;
	
    Power_EepromInit();
    Power_SetWorkHour(Power_EepromGetWorkHour());
    Power_ClearMaxMin();
}
