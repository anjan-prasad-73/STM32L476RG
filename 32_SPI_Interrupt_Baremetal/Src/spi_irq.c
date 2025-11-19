#include "spi_irq.h"
#include "spi_master.h"   // uses spi1_master_init()
#include "system_clock.h"

// PB3 LED helper (we toggle PB3 in IRQ)
static inline void led_init_pb3(void)
{
    // Enable GPIOB clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    // Configure PB3 as output (01)
    GPIOB->MODER &= ~(3U << (3*2));
    GPIOB->MODER |=  (1U << (3*2));
    // Push-pull
    GPIOB->OTYPER &= ~(1U << 3);
    // Medium speed
    GPIOB->OSPEEDR &= ~(3U << (3*2));
    GPIOB->OSPEEDR |=  (1U << (3*2));
    // No pull
    GPIOB->PUPDR &= ~(3U << (3*2));
    // Ensure LED off
    GPIOB->BSRR = (1U << (3 + 16)); // reset
}

// SPI1 IRQ: called when RXNE set (we enabled RXNEIE)
void SPI1_IRQHandler(void)
{
    // Check RXNE flag and read data
    if (SPI1->SR & SPI_SR_RXNE) {
        uint8_t val = *(__IO uint8_t *)&SPI1->DR; // clear RXNE by reading DR
        (void)val; // optionally use val
        // Toggle LED on PB3
        GPIOB->ODR ^= (1U << 3);
    }

    // (Optional) handle other SPI interrupts here (OVR, MODF etc.)
    // Clear overrun: if OVR set, need to read DR and SR sequence to clear; not expected in loopback
}

void spi1_irq_init(void)
{
    // Init system clock (call from main normally; safe to call again)
    // SystemClock_HSI16();  // don't call here if main already set clock

    // Init LED on PB3
    led_init_pb3();

    // Initialize SPI1 (pins, CR1/CR2) - this enables SPI peripheral
    spi1_master_init();

    // Enable RXNE interrupt in CR2
    SPI1->CR2 |= SPI_CR2_RXNEIE;

    // Configure NVIC for SPI1
    NVIC_SetPriority(SPI1_IRQn, 3);
    NVIC_EnableIRQ(SPI1_IRQn);
}
