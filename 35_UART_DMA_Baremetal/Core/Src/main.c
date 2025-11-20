/* main.c - STM32L476RG - UART2 TX using DMA1 Channel7 via DMA1_CSELR (no DMAMUX) */
#include "stm32l476xx.h"
#include <string.h>

#define SYSCLK_HZ 16000000U
const char msg[] = "Hello DMA UART from L476 using DMA1_CSELR (CH7->USART2)!\r\n";

/* --- Force HSI and switch SYSCLK to HSI16 --- */
void Clock_HSI_Enable(void)
{
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

/* --- USART2 GPIO (PA2 = TX, PA3 = RX) --- */
void uart2_gpio_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    /* PA2, PA3 -> AF mode */
    GPIOA->MODER &= ~((3<<4)|(3<<6));
    GPIOA->MODER |=  (2<<4)|(2<<6);
    /* AF7 for USART2 */
    GPIOA->AFR[0] &= ~((0xF<<8)|(0xF<<12));
    GPIOA->AFR[0] |=  (7<<8)|(7<<12);
}

/* --- USART2 init (115200) --- */
void uart2_init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    USART2->CR1 = 0;
    USART2->BRR = SYSCLK_HZ / 115200U;
    USART2->CR3 = 0;
    USART2->CR1 |= USART_CR1_TE;
    USART2->CR1 |= USART_CR1_UE;
    while(!(USART2->ISR & USART_ISR_TEACK));
}

/* --- Small polling print helper --- */
void uart2_putc_poll(char c)
{
    while(!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = (uint8_t)c;
}
void uart2_print_poll(const char *s)
{
    while(*s) uart2_putc_poll(*s++);
}

/* --- Map DMA1 Channel7 -> USART2_TX using DMA1_CSELR (C7S = 4) --- */
void dma1_cselr_map_ch7_usart2tx(void)
{
    /* Enable DMA1 clock so CSELR register is accessible */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* Clear C7S bits (4 bits per channel, channel7 offset = 7*4 = 28) */
    DMA1_CSELR->CSELR &= ~(0xFUL << (7U * 4U));

    /* Set C7S = 4 -> peripheral request 4 (USART2_TX for CSELR mapping) */
    DMA1_CSELR->CSELR |=  (4U   << (7U * 4U));
}

/* --- Configure DMA1 Channel7 for memory->peripheral TX --- */
void dma1_ch7_start_tx(const uint8_t *buf, uint32_t len)
{
    /* Ensure DMA1 clock enabled (done above also) */
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    /* Disable channel before config */
    DMA1_Channel7->CCR &= ~DMA_CCR_EN;
    while(DMA1_Channel7->CCR & DMA_CCR_EN);

    /* Set peripheral addr = USART2->TDR, memory = buf, count = len */
    DMA1_Channel7->CPAR  = (uint32_t)&USART2->TDR;
    DMA1_Channel7->CMAR  = (uint32_t)buf;
    DMA1_Channel7->CNDTR = len;

    /* CCR: memory increment, DIR = mem->periph, priority medium (bits 12-13 = 01) */
    DMA1_Channel7->CCR = DMA_CCR_MINC    /* memory increment */
                       | DMA_CCR_DIR     /* memory -> peripheral */
                       | (1U << 12);     /* medium priority (01) */

    /* Enable USART2 DMA request for TX and then enable DMA channel */
    USART2->CR3 |= USART_CR3_DMAT;
    DMA1_Channel7->CCR |= DMA_CCR_EN;
}

int main(void)
{
    Clock_HSI_Enable();
    uart2_gpio_init();
    uart2_init();

    uart2_print_poll("UART polling OK\r\n");

    /* Map CH7 -> USART2_TX using CSELR and start DMA */
    dma1_cselr_map_ch7_usart2tx();
    dma1_ch7_start_tx((const uint8_t*)msg, (uint32_t)strlen(msg));

    uart2_print_poll("DMA started, waiting CNDTR->0...\r\n");

    /* Wait for DMA to finish moving data to peripheral */
    while(DMA1_Channel7->CNDTR != 0);

    /* Wait until last byte is shifted out */
    while(!(USART2->ISR & USART_ISR_TC));
    USART2->ICR = USART_ICR_TCCF;

    uart2_print_poll("DMA done!\r\n");

    while(1) { __WFI(); }
}
