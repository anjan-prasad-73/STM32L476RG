#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32l476xx.h"
#include <stdint.h>

#define I2C_WRITE   0
#define I2C_READ    1

void i2c1_init(void);
void i2c1_start(uint8_t addr, uint8_t direction);
void i2c1_write(uint8_t data);
uint8_t i2c1_read_ack(void);
uint8_t i2c1_read_nack(void);
void i2c1_stop(void);

#endif
