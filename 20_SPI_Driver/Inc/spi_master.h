#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include "stm32l476xx.h"
#include <stdint.h>

void spi1_master_init(void);
uint8_t spi1_transfer(uint8_t tx);

#endif
