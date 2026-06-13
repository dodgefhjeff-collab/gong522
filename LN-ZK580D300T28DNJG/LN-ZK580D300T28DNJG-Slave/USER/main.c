

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "queue.h"
#include "semphr.h"
#include "gd32c10x.h"
#include "GPIO.H"
#include <stdio.h>
#include "usart.h"
#include "delay.h"
#include <math.h>
#include "adc.h"
#include "dma.h"
#include "nvic.H"
#include "power.h"
#include "timer.h"
#include "pwm.h"
// #include "my_printf.h"
#include "iap_APP.h"
#include "iap_crc.h"
#include "CRC.h"
/*------------------궨-----¼-------------*/

/* 宏定义说明：event_bit_VO1，FreeRTOS事件组事件位宏。 */
#define event_bit_VO1 (1 << 0)  // VO1ر
/* 宏定义说明：event_bit_VO2，FreeRTOS事件组事件位宏。 */
#define event_bit_VO2 (1 << 1)  // VO2ر
/* 宏定义说明：event_bit_VO3，FreeRTOS事件组事件位宏。 */
#define event_bit_VO3 (1 << 2)  // VO3ر
/* 宏定义说明：event_bit_test，FreeRTOS事件组事件位宏。 */
#define event_bit_test (1 << 3) // Ա

/* 宏定义说明：event_bit_ERR_TEMP，FreeRTOS事件组事件位宏。 */
#define event_bit_ERR_TEMP (1 << 4)   // ¹
/* 宏定义说明：event_bit_ERR_VO1_OC，FreeRTOS事件组事件位宏。 */
#define event_bit_ERR_VO1_OC (1 << 5) // VO1
/* 宏定义说明：event_bit_ERR_VO2_OC，FreeRTOS事件组事件位宏。 */
#define event_bit_ERR_VO2_OC (1 << 6) // VO2
/* 宏定义说明：event_bit_ERR_VO3_OC，FreeRTOS事件组事件位宏。 */
#define event_bit_ERR_VO3_OC (1 << 7) // VO3
/* 宏定义说明：ALL_ERROR_BITS，FreeRTOS事件组事件位宏。 */
#define ALL_ERROR_BITS (event_bit_ERR_TEMP | event_bit_ERR_VO1_OC | event_bit_ERR_VO3_OC |event_bit_ERR_VO2_OC)


extern uint16_t adc1_buf[adc1_order_num][adc1_ch_num];
// ȼԽȼԽ
/* 宏定义说明：ADC_TASK_PRIO，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_TASK_PRIO 3
/* 宏定义说明：START_TASK_PRIO，FreeRTOS任务优先级宏，数值越大优先级越高。 */
#define START_TASK_PRIO 1
/* 宏定义说明：TEST_TASK_PRIO，FreeRTOS任务优先级宏，数值越大优先级越高。 */
#define TEST_TASK_PRIO 1
/* 宏定义说明：USART_TASK_PRIO，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART_TASK_PRIO 5
/* 宏定义说明：LED_TASK_PRIO，FreeRTOS任务优先级宏，数值越大优先级越高。 */
#define LED_TASK_PRIO 4

// ջС
/* 宏定义说明：ADC_STK_SIZE，ADC采样、通道数量或滤波深度相关宏。 */
#define ADC_STK_SIZE 150
/* 宏定义说明：TEST_STK_SIZE，FreeRTOS任务堆栈大小宏。 */
#define TEST_STK_SIZE 128
/* 宏定义说明：START_STK_SIZE，FreeRTOS任务堆栈大小宏。 */
#define START_STK_SIZE 200
/* 宏定义说明：USART_STK_SIZE，串口/协议缓存、外设或SLIP处理相关宏。 */
#define USART_STK_SIZE 128

// 
/* 变量说明：ADCTask_Handler，FreeRTOS任务句柄变量，用于创建、删除或管理对应任务。 */
TaskHandle_t ADCTask_Handler;
/* 变量说明：StartTask_Handler，FreeRTOS任务句柄变量，用于创建、删除或管理对应任务。 */
TaskHandle_t StartTask_Handler;
/* 变量说明：TESTTask_Handler，FreeRTOS任务句柄变量，用于创建、删除或管理对应任务。 */
TaskHandle_t TESTTask_Handler;
/* 变量说明：UARTTask_Handler，FreeRTOS任务句柄变量，用于创建、删除或管理对应任务。 */
TaskHandle_t UARTTask_Handler;

// 
/* 变量说明：SendMessgMutex，FreeRTOS互斥量句柄变量，用于保护共享缓冲区。 */
xSemaphoreHandle SendMessgMutex = NULL;

// ź
/* 变量说明：SemaphoerCapOVPHandle，FreeRTOS信号量句柄变量，用于中断和任务同步。 */
xSemaphoreHandle SemaphoerCapOVPHandle = NULL; // ADCŹ

// ¼
/* 变量说明：EventHandle，EventHandle变量，用于保存当前模块运行过程中的状态或临时数据。 */
EventGroupHandle_t EventHandle = NULL;

// о
/* 变量说明：Queuehandler_USART，FreeRTOS队列句柄变量，用于任务间传递消息。 */
QueueHandle_t Queuehandler_USART = NULL;
/* 变量说明：Queuehandler_ModOutNum，FreeRTOS队列句柄变量，用于任务间传递消息。 */
QueueHandle_t Queuehandler_ModOutNum = NULL;

// Ϣлôָķʽ
/* 变量说明：Queue_USART，Queue_USART变量，用于保存当前模块运行过程中的状态或临时数据。 */
queue_data Queue_USART; 



/* 函数声明说明：SYS_init，28PIN从机系统初始化入口。 */
void SYS_init(void);
/* 函数声明说明：start_task，创建业务任务后删除自身的启动任务。 */
void start_task(void *pvParameters); // ʼ
/* 函数声明说明：test_task，test_task函数，保持原工程接口并完成对应模块处理。 */
void test_task(void *pvParameters);  // 
/* 函数声明说明：USART_task，周期执行串口发送或接收处理任务。 */
void USART_task(void *pvParameters); // USART
/* 函数声明说明：ADC_task，周期执行ADC采样换算任务。 */
void ADC_task(void *pvParameters);   // ADC
/*******************************************************************************
** 函数名称: main
** 功能描述: main函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
int main()
{

//    IAP_APP_Init();     // Ӧóʼ
    SYS_init();         // ϵͳʼ
    CRC32TableCreate(); // CRCУı
		
    while (1)
    {
        // 
        if (SendMessgMutex == NULL)
        {
            SendMessgMutex = xSemaphoreCreateMutex();
        }
        // ֵź
        if (SemaphoerCapOVPHandle == NULL)
        {
            SemaphoerCapOVPHandle = xSemaphoreCreateBinary();
        }

        // ¼
        if (EventHandle == NULL)
        {
            EventHandle = xEventGroupCreate();
        }

        // 
        if (Queuehandler_USART == NULL)
        {
            Queuehandler_USART = xQueueCreate(1, sizeof(Queue_USART)); // 
        }
        if (Queuehandler_ModOutNum == NULL)
        {
            Queuehandler_ModOutNum = xQueueCreate(1, 1); // 
        }

        // ʼ
        xTaskCreate((TaskFunction_t)start_task,
                    (const char *)"start_task",
                    (uint16_t)START_STK_SIZE,
                    (void *)NULL,
                    (UBaseType_t)START_TASK_PRIO,
                    (TaskHandle_t *)&StartTask_Handler);
        // 
        vTaskStartScheduler();
    }
}

/*******************************************************************************
** : ADC_task
** : 20msڲɼ/˲/㣬48PINͨPA3·Ŀ
********************************************************************************/
/*******************************************************************************
** 函数名称: ADC_task
** 功能描述: 周期执行ADC采样换算任务。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void ADC_task(void *pvParameters)
{
    /* 变量说明：xLastWakeTime，xLastWakeTime变量，用于保存当前模块运行过程中的状态或临时数据。 */
    TickType_t xLastWakeTime;
    /* 变量说明：xFrequency，xFrequency变量，用于保存当前模块运行过程中的状态或临时数据。 */
    const TickType_t xFrequency = 20; // 20msɼ
    xLastWakeTime = xTaskGetTickCount();    // ȡǰʱΪ׼
    
    while (1)
    {
        compute_Voltage(); // ɼ˲
        DataConversion();  
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/*******************************************************************************
** 函数名称: test_task
** 功能描述: test_task函数，保持原工程接口并完成对应模块处理。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void test_task(void *pvParameters)
{
    /* 变量说明：strbuf1，strbuf1变量，用于保存当前模块运行过程中的状态或临时数据。 */
    char strbuf1[2000];
    while (1)
    {

        if (xEventGroupWaitBits(EventHandle, event_bit_test, pdFALSE, pdTRUE, portMAX_DELAY))
        {
            test();
            vTaskList(strbuf1);
            printf("\n        ǰ״̬ ȼ Сʣ ˳");
            printf("\n%s\n", strbuf1);

            vTaskGetRunTimeStats(strbuf1);
            printf("\n           ʱ             ռ");
            printf("\n%s\n", strbuf1);
            vTaskDelay(500);
        }
				continue;
    }
}


/*******************************************************************************
** : USART_task
** : 28PINͨPA2Ͳɼ֡48PIN֡ʽ55 AA ... 66 BB
** ˵    : DataConversion()USART1_TX_BUFֻDMA͡
********************************************************************************/
/*******************************************************************************
** 函数名称: USART_task
** 功能描述: 周期执行串口发送或接收处理任务。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void USART_task(void *pvParameters)
{

    /* 变量说明：xLastWakeTime，xLastWakeTime变量，用于保存当前模块运行过程中的状态或临时数据。 */
    TickType_t xLastWakeTime;
    /* 变量说明：xFrequency，xFrequency变量，用于保存当前模块运行过程中的状态或临时数据。 */
    const TickType_t xFrequency = 20;
    xLastWakeTime = xTaskGetTickCount();    // ȡǰʱΪ׼
    
    while (1)
    {
        Power_ProcessHostCommand();
        // ȡ
        if (xSemaphoreTake(SendMessgMutex, portMAX_DELAY) == pdTRUE)
        {
            usart_Sen_dma(1, USART1_TX_BUF, POWER_UPLOAD_FRAME_LEN);
						vTaskDelay(15);         
            // ͷŻ
            xSemaphoreGive(SendMessgMutex);
        }
        
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

/*******************************************************************************
** 函数名称: start_task
** 功能描述: 创建业务任务后删除自身的启动任务。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void start_task(void *pvParameters)
{
    // ٽ
    taskENTER_CRITICAL();
    // ADCɼ
    if (xTaskCreate((TaskFunction_t)ADC_task,   // 
                    (const char *)"ADC_task",   // 
                    (uint16_t)ADC_STK_SIZE,     // ջС
                    (void *)NULL,               // 
                    (UBaseType_t)ADC_TASK_PRIO, // ȼ
                    (TaskHandle_t *)&ADCTask_Handler) != pdPASS)
    {
        printf("ADC_task ʧ\n");
    }; // 

    if (xTaskCreate((TaskFunction_t)test_task,   // 
                    (const char *)"TEST_task",   // 
                    (uint16_t)TEST_STK_SIZE,     // ջС
                    (void *)NULL,                // 
                    (UBaseType_t)TEST_TASK_PRIO, // ȼ
                    (TaskHandle_t *)&TESTTask_Handler) != pdPASS)
    {
        printf("TEST_task ʧ\n");
    }; // 
    if (xTaskCreate((TaskFunction_t)USART_task,   // 
                    (const char *)"USART_task",   // 
                    (uint16_t)USART_STK_SIZE,     // ջС
                    (void *)NULL,                 // 
                    (UBaseType_t)USART_TASK_PRIO, // ȼ
                    (TaskHandle_t *)&UARTTask_Handler) != pdPASS)
    {
        printf("USART2_task ʧ\n");
    }; // 


    taskEXIT_CRITICAL();
    vTaskDelete(StartTask_Handler);
}



/*******************************************************************************
** 函数名称: SYS_init
** 功能描述: 28PIN从机系统初始化入口。
** 参数说明: 参见函数形参定义，保持原工程接口不改变。
** 返回说明: 参见函数返回类型。
********************************************************************************/
void SYS_init()
{

    GPIO_init_all(); // IOʼ
    delay_Init();    // ʱʼ
    NVIC_init_all(); // ʼж
    MYDMA_Config();  // 쳣λ
    adc_config();    // ADC
    uart1_init(115200);
    /* 10kHz PWM：72MHz / (7+1) / (899+1) = 10000Hz，占空比比较值范围 0~899 */
    pwm_config(899, 7);
    timer1_config();
		timer2_config();	

}
