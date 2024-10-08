#include <ctype.h>
#include <string.h>

#include "uart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define COM_UART        USART2
#define COM_IRQ_NUM     USART2_IRQn
#define COM_IRQ_HANDLER USART2_IRQHandler

#define UART_TX_BUF_SIZE 0x800u
#define UART_RX_BUF_SIZE 0x20u

//USART2 TX(PA2), RX(PA3))
UART_HandleTypeDef uart_handle =
{
    .Instance          = COM_UART,
    .Init.BaudRate     = 115200u,
    .Init.WordLength   = UART_WORDLENGTH_8B,
    .Init.StopBits     = UART_STOPBITS_1,
    .Init.Parity       = UART_PARITY_NONE,
    .Init.HwFlowCtl    = UART_HWCONTROL_NONE,
    .Init.Mode         = UART_MODE_TX_RX,
    .Init.OverSampling = UART_OVERSAMPLING_16,
};

#define TX_MASK(idx)    ((idx) & (UART_TX_BUF_SIZE - 1U))

static uint8_t _tx_buf[UART_TX_BUF_SIZE];
static volatile uint32_t _tx_buf_head;
static volatile uint32_t _tx_buf_tail;

static uint8_t _rx_buf[UART_RX_BUF_SIZE];
static uint32_t _rx_buf_head;

static bool _is_input_ready = false;

static void _handle_input(uint8_t byte)
{
    uint32_t head = _rx_buf_head;

    if ((0 != isalnum(byte)) || (' ' == byte)) { // Alphanumerics or Space
        if ((false == _is_input_ready) && (head < UART_RX_BUF_SIZE)) {
            _rx_buf[head] = byte;
            _rx_buf_head = head + 1u;
            uart_send_byte(byte);
        }
    } else if (('\b' == byte) || (0x7Fu == byte)) { // Backspace
        if (head > 0u) {
            _rx_buf_head = head - 1u;
            uart_send_byte('\b');
            uart_send_byte(' ');
            uart_send_byte('\b');
        }
    } else if (('\r' == byte) || ('\n' == byte)) { // Enter
        // Note: Also return zero length lines to show that handling chain is operational
        _rx_buf[head] = 0u;
        _rx_buf_head = 0;
        _is_input_ready = true;
        uart_send_byte('\r');
        uart_send_byte('\n');
    } // Ignore everything else
}

// -- Public Interface --
uint32_t uart_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART2_CLK_ENABLE();

    GPIO_InitTypeDef  gpio_init_structure;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_PULLUP;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Alternate = GPIO_AF7_USART2;

    // USART2 TX(PA2)
    gpio_init_structure.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    // USART2 RX(PA3)
    gpio_init_structure.Pin = GPIO_PIN_3;
    HAL_GPIO_Init(GPIOA, &gpio_init_structure);

    if (HAL_OK != HAL_UART_Init(&uart_handle)) {
        return HAL_ERROR;
    }

    // enable RX interrupt
    COM_UART->CR1 |= (USART_CR1_RXNEIE | USART_CR1_RE);

    HAL_NVIC_SetPriority(COM_IRQ_NUM, 0, 1);
    HAL_NVIC_EnableIRQ(COM_IRQ_NUM);

    return HAL_OK;
}

void uart_send_byte(const uint8_t b)
{
    uint32_t head = _tx_buf_head;
    uint32_t tail = _tx_buf_tail;

    if ((head - tail) < UART_TX_BUF_SIZE) {
        _tx_buf[TX_MASK(head)] = b;
        _tx_buf_head = head + 1u;
    }

    COM_UART->CR1 |= USART_CR1_TXEIE;
}

void uart_send(const uint8_t* buf, uint16_t len)
{
    while (len-- > 0u) {
        uart_send_byte(*buf++);
    }
}

void uart_send_str(const char* buf)
{
    while (0u != *buf) {
        uart_send_byte((const uint8_t)*buf++);
    }
}

bool uart_get_line(line_buf_t* line)
{
    if (true == _is_input_ready) {
        memcpy(line->buf, &_rx_buf[0], sizeof(((line_buf_t){0}).buf));
        _is_input_ready = false;
        return true;
    }
    return false;
}

void COM_IRQ_HANDLER(void)
{
    uint32_t status = COM_UART->SR;

    if (0u != (status & USART_SR_TXE)) {
        uint32_t head = _tx_buf_head;
        uint32_t tail = _tx_buf_tail;

        if (tail == head) {
            COM_UART->CR1 &= ~ USART_CR1_TXEIE;
        } else {
            COM_UART->DR = _tx_buf[TX_MASK(tail)];
            _tx_buf_tail = tail + 1u;
        }
    }

    if (0u != (status & USART_SR_RXNE)) {
        uint32_t byte = COM_UART->DR;
        _handle_input((uint8_t)byte);
    }
}
