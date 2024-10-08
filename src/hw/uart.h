#ifndef _UART_H_
#define _UART_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t buf[0x20u];
 } line_buf_t;

uint32_t uart_init(void);

void uart_send_byte(const uint8_t b);
void uart_send_str(const char* buf);
void uart_send(const uint8_t* buf, uint16_t len);

bool uart_get_line(line_buf_t* line);

#endif // _UART_H_
