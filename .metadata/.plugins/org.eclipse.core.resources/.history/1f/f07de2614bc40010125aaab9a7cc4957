#include "stm32l476xx.h"

void SystemClock_HSI16(void)
{
    // 1) Enable HSI16
    RCC->CR |= RCC_CR_HSION;

    while (!(RCC->CR & RCC_CR_HSIRDY));   // Wait until HSI ready

    // 2) FLASH latency (0 wait states for 16 MHz)
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_0WS;

    // 3) Select HSI16 as system clock
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    // Wait until HSI used as SYSCLK
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}
