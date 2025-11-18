#include "uart_irq.h"

volatile uint8_t uart_rx_buffer[64];
volatile uint8_t uart_rx_index = 0;
volatile uint8_t uart_byte_received = 0;

void usart2_irq_init(uint32_t baud)
{
    // Enable clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    // PA2 = TX, PA3 = RX (AF7)
    GPIOA->MODER &= ~((3U<<(2*2)) | (3U<<(3*2)));
    GPIOA->MODER |=  ((2U<<(2*2)) | (2U<<(3*2)));

    GPIOA->AFR[0] &= ~((0xFU<<(2*4)) | (0xFU<<(3*4)));
    GPIOA->AFR[0] |=  ((7U<<(2*4)) | (7U<<(3*4)));

    // Disable USART before config
    USART2->CR1 &= ~USART_CR1_UE;

    // Clock = 16 MHz (HSI16)
    uint32_t div = 16000000 / baud;
    USART2->BRR = div;

    // Enable TX, RX, RX interrupt
    USART2->CR1 |= USART_CR1_TE |
                   USART_CR1_RE |
                   USART_CR1_RXNEIE;

    // Enable USART
    USART2->CR1 |= USART_CR1_UE;

    // Enable interrupt in NVIC (THIS MUST MATCH VECTOR TABLE!)
    NVIC_SetPriority(USART2_IRQn, 1);
    NVIC_EnableIRQ(USART2_IRQn);
}

void usart2_write(uint8_t data)
{
    while (!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = data;
}

void USART2_IRQHandler(void)
{
    if (USART2->ISR & USART_ISR_RXNE)
    {
        uint8_t c = USART2->RDR;
        uart_rx_buffer[uart_rx_index++] = c;

        uart_byte_received = 1;   // signal to main()
    }
}
