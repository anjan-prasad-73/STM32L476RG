// gpio_driver.h
#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include "stm32l476xx.h"
#include <stdint.h>

typedef enum {
    GPIO_MODE_INPUT  = 0x0,
    GPIO_MODE_OUTPUT = 0x1,
    GPIO_MODE_AF     = 0x2,
    GPIO_MODE_ANALOG = 0x3
} gpio_mode_t;

typedef enum {
    GPIO_PULL_NONE = 0x0,
    GPIO_PULL_UP   = 0x1,
    GPIO_PULL_DOWN = 0x2
} gpio_pull_t;

void gpio_enable_port_clock(GPIO_TypeDef *port);
void gpio_config_pin(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_pull_t pull);
void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t value);
uint8_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin);
void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin);

#endif // GPIO_DRIVER_H
