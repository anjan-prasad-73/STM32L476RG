#include "uart.h"

void usart2_init(uint32_t baud)
{
    // Enable clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    // PA2 (TX), PA3 (RX) -> AF7
    GPIOA->MODER &= ~((3U<<(2*2)) | (3U<<(3*2)));
    GPIOA->MODER |=  ((2U<<(2*2)) | (2U<<(3*2)));

    GPIOA->AFR[0] &= ~((0xF<<(2*4)) | (0xF<<(3*4)));
    GPIOA->AFR[0] |=  ((7U<<(2*4)) | (7U<<(3*4)));

    // Speed
    GPIOA->OSPEEDR |= (3U<<(2*2)) | (3U<<(3*2));

    // Disable USART before config
    USART2->CR1 &= ~USART_CR1_UE;

    // Assuming PCLK1 = 16 MHz (HSI16)
    uint32_t periphclk = 16000000;
    USART2->BRR = periphclk / baud;

    // Enable TX, RX
    USART2->CR1 = USART_CR1_TE | USART_CR1_RE;

    // Enable USART
    USART2->CR1 |= USART_CR1_UE;
}

void usart2_send(uint8_t data)
{
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = data;

    while (!(USART2->ISR & USART_ISR_TC));
}

uint8_t usart2_recv(void)
{
    while (!(USART2->ISR & USART_ISR_RXNE));
    return USART2->RDR;
}
