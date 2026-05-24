/* 文件：usart.c，GD32C103CBT6 48PIN USART初始化、SLIP收发、CRC校验 */

#include "sys.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include <string.h>

uint8_t USART_RX_STA = 0;
uint8_t USART2_RX_STA = 0;
uint8_t USART_RX_BUF[USART_REC_IDLE_LEN];
uint8_t USART_TX_BUF[USART_TX_LEN];
uint8_t USART2_RX_BUF[USART2_REC_IDLE_LEN];
uint8_t USART2_TX_BUF[USART2_TX_LEN];
static uint8_t uart_slip_tx_buff[UART_SLIP_TX_MAX_LEN];
static volatile uint8_t uart_protocol_tx_busy = 0;
static volatile uint16_t uart_rx_head = 0;
static volatile uint16_t uart_rx_tail = 0;
static uint32_t uart_get_dma_ch(uint8_t uartx)
{
    switch (uartx)
    {
    case POWER_LINK_UART_INDEX:
        
        return DMA_CH6;   // PA2/PA3内部通信口：GD32 USART1_TX -> DMA0_CH6

    case POWER_EXT_UART_INDEX:
    default:
        
        return DMA_CH1;   // PB10/PB11对外通信口：GD32 USART2_TX -> DMA0_CH1
    }
}

static uint8_t uart_dma_tx_idle(uint8_t uartx)
{
    uint32_t ch = uart_get_dma_ch(uartx);

    if (((DMA_CHCTL(DMA0, ch) & DMA_CHXCTL_CHEN) == 0) || (DMA_CHCNT(DMA0, ch) == 0))
    {
        return 1;
    }
    return 0;
}

static uint8_t uart_dma_wait_idle(uint8_t uartx)
{
    
    uint32_t timeout = 720000;

    while (uart_dma_tx_idle(uartx) == 0)
    {
        if (timeout-- == 0)
        {
            return 0;
        }
    }
    return 1;
}

static uint8_t uart_take_tx_lock(void)
{
    
    uint32_t timeout = 720000;

    while (timeout--)
    {
        __disable_irq();
        if (uart_protocol_tx_busy == 0)
        {
            uart_protocol_tx_busy = 1;
            __enable_irq();
            return 1;
        }
        __enable_irq();
    }

    return 0;
}

static void uart_release_tx_lock(void)
{
    __disable_irq();
    uart_protocol_tx_busy = 0;
    __enable_irq();
}

static void uart_gpio_init(void)
{
    /* PA2/PA3：两个单片机之间通信，GD32C103库中为USART1。 */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
}

static void uart2_gpio_init(void)
{
    /* PB10/PB11：对外管理通信口，GD32C103库中为USART2。 */
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

    /* PB10/PB11是USART2默认映射，关闭可能的重映射，避免串口号正确但引脚无波形。 */
#ifdef GPIO_USART2_FULL_REMAP
    gpio_pin_remap_config(GPIO_USART2_FULL_REMAP, DISABLE);
#endif
#ifdef GPIO_USART2_PARTIAL_REMAP
    gpio_pin_remap_config(GPIO_USART2_PARTIAL_REMAP, DISABLE);
#endif

    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);      // PB10 -> USART2_TX
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);// PB11 -> USART2_RX
}

void uart_init(uint32_t bound)
{
    uart_gpio_init();
    rcu_periph_clock_enable(RCU_USART1);

    usart_deinit(POWER_UART1);
    usart_baudrate_set(POWER_UART1, bound);
    usart_word_length_set(POWER_UART1, USART_WL_8BIT);
    usart_stop_bit_set(POWER_UART1, USART_STB_1BIT);
    usart_parity_config(POWER_UART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(POWER_UART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(POWER_UART1, USART_CTS_DISABLE);
    usart_receive_config(POWER_UART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(POWER_UART1, USART_TRANSMIT_ENABLE);

    /* PA2/PA3为双MCU内部通信口，采用RBNE中断接收原始字节。 */
    uart_clear_rx_buff();
    usart_dma_transmit_config(POWER_UART1, USART_DENT_DISABLE);
    usart_dma_receive_config(POWER_UART1, USART_DENR_DISABLE);
    usart_interrupt_disable(POWER_UART1, USART_INT_IDLE);
    usart_interrupt_enable(POWER_UART1, USART_INT_RBNE);
    usart_enable(POWER_UART1);
}

void uart2_init(uint32_t bound)
{
    uart2_gpio_init();
    rcu_periph_clock_enable(RCU_USART2);

    usart_deinit(POWER_UART2);
    usart_baudrate_set(POWER_UART2, bound);
    usart_word_length_set(POWER_UART2, USART_WL_8BIT);
    usart_stop_bit_set(POWER_UART2, USART_STB_1BIT);
    usart_parity_config(POWER_UART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(POWER_UART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(POWER_UART2, USART_CTS_DISABLE);
    usart_receive_config(POWER_UART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(POWER_UART2, USART_TRANSMIT_ENABLE);

    /* 对外协议口采用RBNE中断逐字节接收SLIP，发送采用阻塞方式。；这样可以避开DMA接收不定长SLIP帧时的IDLE长度计算问题。 */
    usart_dma_transmit_config(POWER_UART2, USART_DENT_DISABLE);
    usart_dma_receive_config(POWER_UART2, USART_DENR_DISABLE);
    usart_interrupt_disable(POWER_UART2, USART_INT_IDLE);
    usart_interrupt_enable(POWER_UART2, USART_INT_RBNE);
    usart_enable(POWER_UART2);
}

void uart2_force_gpio_test(void)
{
    uint32_t i, j;

    /* 纯GPIO强制翻转PB10，用于判断PB10物理引脚/隔离器/接线是否有波形。 */
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    for (i = 0; i < 20; i++)
    {
        gpio_bit_set(GPIOB, GPIO_PIN_10);
        for (j = 0; j < 8000; j++) { __NOP(); }
        gpio_bit_reset(GPIOB, GPIO_PIN_10);
        for (j = 0; j < 8000; j++) { __NOP(); }
    }

    uart2_gpio_init();    // 测试完恢复PB10/PB11为USART2复用
}

/* uart_rx_byte_process：USART1接收中断调用，保存PA2/PA3内部通信字节 */
void uart_rx_byte_process(uint8_t ch)
{
    
    uint16_t next_head;

    next_head = uart_rx_head + 1;
    if (next_head >= USART_REC_IDLE_LEN)
    {
        next_head = 0;
    }

    if (next_head != uart_rx_tail)
    {
        USART_RX_BUF[uart_rx_head] = ch;
        uart_rx_head = next_head;
    }
    else
    {
        USART_RX_STA = 1;      // 接收缓冲满，置位供上层调试
    }
}

/* uart_get_rx_byte：从PA2/PA3内部通信环形缓冲读取1字节 */
uint8_t uart_get_rx_byte(uint8_t *ch)
{
    if (ch == 0)
    {
        return 0;
    }

    if (uart_rx_head == uart_rx_tail)
    {
        return 0;
    }

    __disable_irq();
    if (uart_rx_head != uart_rx_tail)
    {
        *ch = USART_RX_BUF[uart_rx_tail];
        uart_rx_tail++;
        if (uart_rx_tail >= USART_REC_IDLE_LEN)
        {
            uart_rx_tail = 0;
        }
        __enable_irq();
        return 1;
    }
    __enable_irq();

    return 0;
}

void uart_clear_rx_buff(void)
{
    __disable_irq();
    uart_rx_head = 0;
    uart_rx_tail = 0;
    USART_RX_STA = 0;
    __enable_irq();
}

/* uart_send_data：通过PA2/PA3内部通信口发送原始数据 */
void uart_send_data(uint8_t *buff, uint16_t len)
{
    if (buff == 0 || len == 0)
    {
        return;
    }

    Usart_SendArray(POWER_UART1, buff, len);
}

/* uart2_rx_byte_process：USART2_IRQHandler调用；中断内直接完成SLIP反转义并缓存原始协议帧 */

static volatile uint8_t uart2_rx_frame_ready = 0;

static volatile uint16_t uart2_rx_frame_len = 0;

static uint8_t uart2_rx_frame_buf[USART2_REC_IDLE_LEN];

/* uart2_rx_byte_process：USART2接收中断调用，完成外部SLIP帧反转义和缓存 */
void uart2_rx_byte_process(uint8_t ch)
{
    
    static uint8_t rx_started = 0;
    
    static uint8_t esc_flag = 0;
    
    static uint16_t rx_cnt = 0;
    
    static uint8_t rx_work_buf[USART2_REC_IDLE_LEN];
    
    uint8_t data;

    if (ch == UART_SLIP_END)
    {
        if (rx_started == 0)
        {
            rx_started = 1;
            esc_flag = 0;
            rx_cnt = 0;
            return;
        }

        /* 收到帧尾；空帧直接忽略，完整帧交给任务层做长度和CRC检查。 */
        if (rx_cnt >= 8 && uart2_rx_frame_ready == 0)
        {
            memcpy(uart2_rx_frame_buf, rx_work_buf, rx_cnt);
            uart2_rx_frame_len = rx_cnt;
            uart2_rx_frame_ready = 1;
        }

        rx_started = 1;  // SLIP允许连续END，下一个END可直接作为新帧起始
        esc_flag = 0;
        rx_cnt = 0;
        return;
    }

    if (rx_started == 0)
    {
        return;          // 未收到0xC0起始前的杂散字节直接丢弃
    }

    if (esc_flag != 0)
    {
        if (ch == UART_SLIP_ESC_END)
        {
            data = UART_SLIP_END;
        }
        else if (ch == UART_SLIP_ESC_ESC)
        {
            data = UART_SLIP_ESC;
        }
        else
        {
            rx_started = 0;
            esc_flag = 0;
            rx_cnt = 0;
            return;
        }
        esc_flag = 0;
    }
    else
    {
        if (ch == UART_SLIP_ESC)
        {
            esc_flag = 1;
            return;
        }
        data = ch;
    }

    if (rx_cnt < USART2_REC_IDLE_LEN)
    {
        rx_work_buf[rx_cnt++] = data;
    }
    else
    {
        /* 超过1064字节说明长度异常，丢弃当前帧，等待下一个0xC0重新同步。 */
        rx_started = 0;
        esc_flag = 0;
        rx_cnt = 0;
    }
}

uint8_t uart2_get_rx_frame(uint8_t *rx_buff, uint16_t *rx_len)
{
    if (rx_buff == 0 || rx_len == 0)
    {
        return 0;
    }

    if (uart2_rx_frame_ready == 0)
    {
        return 0;
    }

    __disable_irq();
    if (uart2_rx_frame_ready != 0)
    {
        memcpy(rx_buff, uart2_rx_frame_buf, uart2_rx_frame_len);
        *rx_len = uart2_rx_frame_len;
        uart2_rx_frame_ready = 0;
        uart2_rx_frame_len = 0;
        __enable_irq();
        return 1;
    }
    __enable_irq();

    return 0;
}

void Usart_SendByte(uint32_t usartx, uint8_t ch)
{
    usart_data_transmit(usartx, ch);
    while (usart_flag_get(usartx, USART_FLAG_TBE) == RESET)
        ;
}

void Usart_SendArray(uint32_t usartx, uint8_t *array, uint16_t num)
{
    
    uint16_t i;

    if (array == 0)
    {
        return;
    }

    for (i = 0; i < num; i++)
    {
        Usart_SendByte(usartx, array[i]);
    }

    while (usart_flag_get(usartx, USART_FLAG_TC) == RESET)
        ;
}

void Usart_SendString(uint32_t usartx, char *str)
{
    
    unsigned int k = 0;

    if (str == 0)
    {
        return;
    }

    while (*(str + k) != '\0')
    {
        Usart_SendByte(usartx, (uint8_t)*(str + k));
        k++;
    }

    while (usart_flag_get(usartx, USART_FLAG_TC) == RESET)
    {
    }
}

void PB10_PB11_USART2_SendText(const char *str)
{
    if (str == 0)
    {
        return;
    }

    while (*str != '\0')
    {
        Usart_SendByte(POWER_UART2, (uint8_t)(*str));
        str++;
    }

    while (usart_flag_get(POWER_UART2, USART_FLAG_TC) == RESET)
    {
    }
}

void PB10_PB11_USART2_BlockTest(void)
{
    /* 调试用：PB10(USART2_TX)每1秒输出文本，正式协议运行时不要打开。 */
    while (1)
    {
        PB10_PB11_USART2_SendText("PB10/PB11 USART2 OUTPUT TEST\r\n");
        delay_ms(1000);
    }
}

void Usart_SendHalfWord(uint32_t usartx, uint16_t ch)
{
    uint8_t temp_h, temp_l;

    temp_h = (ch & 0XFF00) >> 8;
    temp_l = ch & 0XFF;

    usart_data_transmit(usartx, temp_h);
    while (usart_flag_get(usartx, USART_FLAG_TBE) == RESET)
        ;

    usart_data_transmit(usartx, temp_l);
    while (usart_flag_get(usartx, USART_FLAG_TBE) == RESET)
        ;
}

void usart_Sen_dma(uint8_t uartx, uint8_t *buff, uint16_t len)
{
    switch (uartx)
    {
    case POWER_LINK_UART_INDEX:
        dma_channel_disable(DMA0, DMA_CH6);
        DMA_CHMADDR(DMA0, DMA_CH6) = (uint32_t)buff;
        DMA_CHCNT(DMA0, DMA_CH6) = len;
        dma_channel_enable(DMA0, DMA_CH6);
        break;

    case POWER_EXT_UART_INDEX:
    default:
        dma_channel_disable(DMA0, DMA_CH1);
        DMA_CHMADDR(DMA0, DMA_CH1) = (uint32_t)buff;
        DMA_CHCNT(DMA0, DMA_CH1) = len;
        dma_channel_enable(DMA0, DMA_CH1);
        break;
    }
}

uint16_t uart_read_u16_be(uint8_t *buff)
{
    return ((uint16_t)buff[0] << 8) | buff[1];
}

void uart_write_u16_be(uint8_t *buff, uint16_t data)
{
    buff[0] = (uint8_t)((data >> 8) & 0xFF);
    buff[1] = (uint8_t)(data & 0xFF);
}

__weak uint16_t crc16_ibm(uint8_t *data, uint16_t length)
{
    
    uint8_t i;
    
    uint16_t crc = 0x0000;

    /* 协议写多项式0x8005、初值0；这里按正常高位先算，结果与工程CRC16查表法一致。 */
    while (length--)
    {
        crc ^= ((uint16_t)(*data++) << 8);
        for (i = 0; i < 8; i++)
        {
            if ((crc & 0x8000) != 0)
            {
                crc = (crc << 1) ^ 0x8005;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    
    return crc;
}

uint16_t uart_crc16_8005(uint8_t *data, uint16_t length)
{
    return crc16_ibm(data, length);
}

static uint16_t uart_read_crc16(uint8_t *buff)
{
#if UART_CRC_HIGH_BYTE_FIRST
    return uart_read_u16_be(buff);
#else
    return ((uint16_t)buff[1] << 8) | buff[0];
#endif
}

static void uart_write_crc16(uint8_t *buff, uint16_t crc)
{
#if UART_CRC_HIGH_BYTE_FIRST
    uart_write_u16_be(buff, crc);
#else
    buff[0] = (uint8_t)(crc & 0xFF);
    buff[1] = (uint8_t)((crc >> 8) & 0xFF);
#endif
}

static uint8_t uart_slip_decode(uint8_t *rx_buff, uint16_t rx_len, uint8_t *out_buff, uint16_t *out_len)
{
    
    uint16_t i;
    
    uint16_t j = 0;
    
    uint8_t esc_flag = 0;

    if (rx_len < 2)
    {
        
        return UART_PROTOCOL_ERR_LEN;
    }

    if (rx_buff[0] == UART_SLIP_END)
    {
        i = 1;
    }
    else
    {
        i = 0;
    }

    for (; i < rx_len; i++)
    {
        if (rx_buff[i] == UART_SLIP_END)
        {
            break;
        }

        if (j >= UART_FRAME_MAX_LEN)
        {
            
            return UART_PROTOCOL_ERR_LEN;
        }

        if (esc_flag)
        {
            if (rx_buff[i] == UART_SLIP_ESC_END)
            {
                out_buff[j++] = UART_SLIP_END;
            }
            else if (rx_buff[i] == UART_SLIP_ESC_ESC)
            {
                out_buff[j++] = UART_SLIP_ESC;
            }
            else
            {
                
                return UART_PROTOCOL_ERR_DATA;
            }
            esc_flag = 0;
        }
        else
        {
            if (rx_buff[i] == UART_SLIP_ESC)
            {
                esc_flag = 1;
            }
            else
            {
                out_buff[j++] = rx_buff[i];
            }
        }
    }

    if (esc_flag)
    {
        
        return UART_PROTOCOL_ERR_DATA;
    }

    *out_len = j;
    
    return UART_PROTOCOL_OK;
}

static uint16_t uart_slip_encode(uint8_t *raw_buff, uint16_t raw_len, uint8_t *out_buff, uint16_t out_max)
{
    
    uint16_t i;
    
    uint16_t j = 0;

    if (out_max < 2)
    {
        return 0;
    }

    out_buff[j++] = UART_SLIP_END;
    for (i = 0; i < raw_len; i++)
    {
        if ((j + 2) >= out_max)
        {
            return 0;
        }

        if (raw_buff[i] == UART_SLIP_END)
        {
            out_buff[j++] = UART_SLIP_ESC;
            out_buff[j++] = UART_SLIP_ESC_END;
        }
        else if (raw_buff[i] == UART_SLIP_ESC)
        {
            out_buff[j++] = UART_SLIP_ESC;
            out_buff[j++] = UART_SLIP_ESC_ESC;
        }
        else
        {
            out_buff[j++] = raw_buff[i];
        }
    }
    out_buff[j++] = UART_SLIP_END;

    
    return j;
}

uint8_t uart_protocol_unpack(uint8_t *rx_buff, uint16_t rx_len, uint8_t *frame_buff, uint16_t *frame_len)
{
    
    uint16_t raw_len = 0;
    
    uint16_t data_len;
    
    uint16_t crc_recv;
    
    uint16_t crc_calc;
    
    uint8_t ret;

    if (rx_buff == 0 || frame_buff == 0 || frame_len == 0)
    {
        
        return UART_PROTOCOL_ERR_DATA;
    }

    if (rx_len > 0 && rx_buff[0] == UART_SLIP_END)
    {
        /* 兼容测试函数直接传入带0xC0的完整SLIP帧。 */
        ret = uart_slip_decode(rx_buff, rx_len, frame_buff, &raw_len);
        if (ret != UART_PROTOCOL_OK)
        {
            
            return ret;
        }
    }
    else
    {
        /* 正式中断接收路径已经在ISR中完成SLIP反转义，rx_buff即原始协议帧。 */
        if (rx_len > UART_FRAME_MAX_LEN)
        {
            
            return UART_PROTOCOL_ERR_LEN;
        }
        memcpy(frame_buff, rx_buff, rx_len);
        raw_len = rx_len;
    }

    if (raw_len < 8)
    {
        
        return UART_PROTOCOL_ERR_LEN;
    }

    data_len = uart_read_u16_be(&frame_buff[4]);
    if (data_len != raw_len)
    {
        
        return UART_PROTOCOL_ERR_LEN;
    }

    crc_recv = uart_read_crc16(&frame_buff[raw_len - 2]);
    crc_calc = uart_crc16_8005(frame_buff, raw_len - 2);
    if (crc_recv != crc_calc)
    {
        
        return UART_PROTOCOL_ERR_CRC;
    }

    *frame_len = raw_len - 2;       // 上层处理时不包含CRC两个字节
    
    return UART_PROTOCOL_OK;
}

/* uart_send_protocol_frame：按ICD要求补长度/CRC、SLIP转义后发送外部协议帧 */
uint8_t uart_send_protocol_frame(uint8_t uartx, uint8_t *frame_buff, uint16_t frame_len)
{
    
    uint16_t crc;
    
    uint16_t send_len;

    if (frame_buff == 0 || frame_len < 6 || frame_len + 2 > UART_TX_FRAME_MAX_LEN)
    {
        
        return UART_PROTOCOL_ERR_LEN;
    }

    if (uart_take_tx_lock() == 0)
    {
        
        return UART_PROTOCOL_ERR_DATA;
    }

    if (uart_dma_wait_idle(uartx) == 0)
    {
        uart_release_tx_lock();
        
        return UART_PROTOCOL_ERR_DATA;
    }

    uart_write_u16_be(&frame_buff[4], frame_len + 2);     // 数据长度=帧头+消息体+CRC
    crc = uart_crc16_8005(frame_buff, frame_len);
    uart_write_crc16(&frame_buff[frame_len], crc);

    send_len = uart_slip_encode(frame_buff, frame_len + 2, uart_slip_tx_buff, UART_SLIP_TX_MAX_LEN);
    if (send_len == 0)
    {
        uart_release_tx_lock();
        
        return UART_PROTOCOL_ERR_LEN;
    }

    if (uartx == POWER_LINK_UART_INDEX)
    {
        Usart_SendArray(POWER_UART1, uart_slip_tx_buff, send_len);
    }
    else
    {
        /* 对外协议口PB10/PB11实际是USART2；阻塞发送便于排查回传波形。 */
        Usart_SendArray(POWER_UART2, uart_slip_tx_buff, send_len);
    }

    uart_release_tx_lock();
    
    return UART_PROTOCOL_OK;
}
