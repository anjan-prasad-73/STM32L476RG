// gpio_driver.c
#include "gpio_driver.h"

// Helper: enable AHB2 clock for given GPIO port
void gpio_enable_port_clock(GPIO_TypeDef *port)
{
    if (port == GPIOA) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    else if (port == GPIOB) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    else if (port == GPIOC) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;
    else if (port == GPIOD) RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
    else if (port == GPIOE) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
    else if (port == GPIOF) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOFEN;
    else if (port == GPIOG) RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;
    // Small delay for clock domain
    volatile int i=0; for(i=0;i<10;i++); (void)i;
}

void gpio_config_pin(GPIO_TypeDef *port, uint8_t pin, gpio_mode_t mode, gpio_pull_t pull)
{
    // Configure moder (2 bits per pin)
    uint32_t pos = pin * 2;
    port->MODER &= ~(0x3U << pos);
    port->MODER |= ( (uint32_t)mode << pos );

    // Configure pull-up/pull-down (2 bits per pin)
    uint32_t pupd_pos = pin * 2;
    port->PUPDR &= ~(0x3U << pupd_pos);
    if (pull == GPIO_PULL_UP) port->PUPDR |= (0x1U << pupd_pos);
    else if (pull == GPIO_PULL_DOWN) port->PUPDR |= (0x2U << pupd_pos);

    // Output type for output pins: push-pull (0)
    if (mode == GPIO_MODE_OUTPUT) {
        port->OTYPER &= ~(1U << pin); // push-pull
        // optional speed: medium
        port->OSPEEDR &= ~(0x3U << pos);
        port->OSPEEDR |= (0x1U << pos);
    }
}

void gpio_write_pin(GPIO_TypeDef *port, uint8_t pin, uint8_t value)
{
    if (value)
        port->BSRR = (1U << pin);   // set bit
    else
        port->BSRR = (1U << (pin + 16U)); // reset bit
}

uint8_t gpio_read_pin(GPIO_TypeDef *port, uint8_t pin)
{
    return ( (port->IDR & (1U << pin)) ? 1U : 0U );
}

void gpio_toggle_pin(GPIO_TypeDef *port, uint8_t pin)
{
    if (port->ODR & (1U << pin))
        gpio_write_pin(port, pin, 0);
    else
        gpio_write_pin(port, pin, 1);
}
