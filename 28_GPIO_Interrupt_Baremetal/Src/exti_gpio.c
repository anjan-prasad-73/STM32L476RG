#include "exti_gpio.h"


void exti_configure_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t trigger_falling, uint8_t trigger_rising)
{
// Enable SYSCFG clock
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;


// Determine EXTICR index and position
uint8_t idx = pin / 4; // EXTICR0..EXTICR3
uint8_t pos = (pin % 4) * 4; // 4 bits per pin


// Map port to value for SYSCFG_EXTICR
uint32_t port_val = 0;
if (port == GPIOA) port_val = 0x0;
else if (port == GPIOB) port_val = 0x1;
else if (port == GPIOC) port_val = 0x2;
else if (port == GPIOD) port_val = 0x3;
else if (port == GPIOE) port_val = 0x4;
else if (port == GPIOF) port_val = 0x5;
else if (port == GPIOG) port_val = 0x6;


// Clear and set the bits in SYSCFG->EXTICR[idx]
SYSCFG->EXTICR[idx] &= ~(0xFU << pos);
SYSCFG->EXTICR[idx] |= (port_val << pos);


// Configure trigger selection
if (trigger_falling) EXTI->FTSR1 |= (1U << pin);
else EXTI->FTSR1 &= ~(1U << pin);


if (trigger_rising) EXTI->RTSR1 |= (1U << pin);
else EXTI->RTSR1 &= ~(1U << pin);


// Unmask interrupt
EXTI->IMR1 |= (1U << pin);
}
