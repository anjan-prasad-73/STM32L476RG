#ifndef UART_H
#define UART_H

#include "stm32l476xx.h"
#include <stdint.h>

void usart2_init(uint32_t baud);
void usart2_send(uint8_t data);
uint8_t usart2_recv(void);

#endif
