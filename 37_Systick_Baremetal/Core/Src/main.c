/* 37_Systick_BareMetal.c
   STM32L476RG - SysTick 1ms tick, delay_ms(), and microsecond helper via DWT.
*/

#include "stm32l476xx.h"
#include <stdint.h>

#define SYSCLK_HZ 16000000U
volatile uint32_t ms_ticks = 0;

void SystemClock_Config_HSI(void){
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

void SysTick_Init_1ms(void){
    SysTick->LOAD = (SYSCLK_HZ/1000U) - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}


/* Busy-wait delay in ms */
void delay_ms(uint32_t ms){
    uint32_t target = ms_ticks + ms;
    while(ms_ticks < target);
}

/* DWT microsecond init / delay */
void dwt_delay_init(void){
    /* Enable TRC */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    /* Unlock and enable cycle counter */
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}
void delay_us(uint32_t us){
    uint32_t start = DWT->CYCCNT;
    uint32_t ticks = (SYSCLK_HZ/1000000U) * us;
    while ((DWT->CYCCNT - start) < ticks);
}

int main(void){
    SystemClock_Config_HSI();
    dwt_delay_init();
    SysTick_Init_1ms();

    while(1){

    	delay_ms(1000);

        /* do something every 500 ms */
    }
}
