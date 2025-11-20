#include "stm32l476xx.h"

#define SYSCLK 16000000UL

volatile uint32_t ms_counter = 0;

void Clock_HSI_Enable(void)
{
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;

    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

void GPIO_LED_Init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN; // PA5

    GPIOA->MODER &= ~(3 << (5*2));
    GPIOA->MODER |=  (1 << (5*2));      // output mode

    GPIOA->OTYPER &= ~(1 << 5);
    GPIOA->OSPEEDR |= (3 << (5*2));
}

void TIM2_Init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    /*
       Goal: generate 1ms interrupt

       TIM2 counter clock = SYSCLK = 16 MHz
       We want 1 kHz update → period 1ms

       Compute prescaler & ARR:
       16 MHz / 16000 = 1000 Hz
       So:
         PSC = 15999  → divides 16 MHz to 1000 Hz
         ARR = 1 - 1  → overflow every 1 tick
    */

    TIM2->PSC = 16000 - 1;     // divide 16MHz → 1kHz
    TIM2->ARR = 1 - 1;         // overflow each count

    TIM2->EGR |= TIM_EGR_UG;   // update registers

    TIM2->DIER |= TIM_DIER_UIE; // enable update interrupt
    TIM2->CR1  |= TIM_CR1_CEN;  // start timer

    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
    if(TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF; // clear flag

        ms_counter++;
        if(ms_counter >= 500)   // 500ms
        {
            ms_counter = 0;
            GPIOA->ODR ^= (1 << 5); // toggle LED
        }
    }
}

int main(void)
{
    Clock_HSI_Enable();
    GPIO_LED_Init();
    TIM2_Init();

    while(1)
    {
        __WFI();  // sleep until interrupt
    }
}
