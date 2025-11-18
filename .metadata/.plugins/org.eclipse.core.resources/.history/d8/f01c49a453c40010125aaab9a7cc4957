#include "i2c_driver.h"

#define I2C_WRITE   0
#define I2C_READ    1

void i2c1_init(void)
{
    // Enable GPIOB and I2C1 clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;

    // PB8 = SCL, PB9 = SDA → AF4
    GPIOB->MODER &= ~((3U << (8*2)) | (3U << (9*2)));
    GPIOB->MODER |=  ((2U << (8*2)) | (2U << (9*2)));

    GPIOB->AFR[1] &= ~((0xF << ((8-8)*4)) | (0xF << ((9-8)*4)));
    GPIOB->AFR[1] |=  ((4U << ((8-8)*4)) | (4U << ((9-8)*4)));

    // Pull-up for I2C lines
    GPIOB->PUPDR &= ~((3U << (8*2)) | (3U << (9*2)));
    GPIOB->PUPDR |=  ((1U << (8*2)) | (1U << (9*2)));

    // Open-drain
    GPIOB->OTYPER |= (1U << 8) | (1U << 9);

    // Reset I2C
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_I2C1RST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_I2C1RST;

    // Disable I2C
    I2C1->CR1 &= ~I2C_CR1_PE;

    // Timing register for 16 MHz -> 400kHz Fast Mode
    I2C1->TIMINGR = 0x00300F38;

    // Enable I2C1 peripheral
    I2C1->CR1 |= I2C_CR1_PE;
}

void i2c1_start(uint8_t addr, uint8_t direction)
{
    // Set slave address & direction
    I2C1->CR2 = (addr << 1) |
                (direction << 10) |
                (1U << 16); // Number of bytes = 1 for now

    // Start condition
    I2C1->CR2 |= I2C_CR2_START;

    // Wait until addressed
    while (!(I2C1->ISR & I2C_ISR_TXIS) &&
           !(I2C1->ISR & I2C_ISR_RXNE));
}

void i2c1_write(uint8_t data)
{
    while (!(I2C1->ISR & I2C_ISR_TXIS));
    I2C1->TXDR = data;
}

uint8_t i2c1_read_ack(void)
{
    while (!(I2C1->ISR & I2C_ISR_RXNE));
    return I2C1->RXDR;
}

uint8_t i2c1_read_nack(void)
{
    while (!(I2C1->ISR & I2C_ISR_RXNE));
    I2C1->CR2 |= I2C_CR2_NACK;
    return I2C1->RXDR;
}

void i2c1_stop(void)
{
    I2C1->CR2 |= I2C_CR2_STOP;
    while (I2C1->ISR & I2C_ISR_STOPF);
}
