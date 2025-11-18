#ifndef I2C_IRQ_H
#define I2C_IRQ_H

#include "stm32l476xx.h"
#include <stdint.h>

#define I2C_WRITE  0
#define I2C_READ   1

void i2c1_irq_init(void);
void i2c1_irq_start(uint8_t addr, uint8_t dir);
void i2c1_irq_write(uint8_t data);
void i2c1_irq_read(uint8_t bytes_to_read);

extern volatile uint8_t i2c_rx_buffer[32];
extern volatile uint8_t i2c_rx_index;
extern volatile uint8_t i2c_transfer_done;

#endif
