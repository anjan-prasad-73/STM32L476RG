#ifndef UART_IRQ_H
#define UART_IRQ_H

#include "stm32l476xx.h"
#include <stdint.h>

void usart2_irq_init(uint32_t baud);
void usart2_write(uint8_t data);

extern volatile uint8_t uart_rx_buffer[64];
extern volatile uint8_t uart_rx_index;
extern volatile uint8_t uart_byte_received;

#endif
