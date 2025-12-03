#include "stm32l476xx.h"
#include <stdint.h>

#define SYSCLK_HZ 16000000U

volatile uint32_t ms_ticks = 0;

/* ----------- Fix UART Clock: Switch to HSI 16 MHz ----------- */
void Clock_HSI_Enable(void)
{
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |=  RCC_CFGR_SW_HSI;

    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

/* ----------- UART2 GPIO (PA2 TX) ----------- */
void uart2_gpio_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    GPIOA->MODER &= ~((3<<4)|(3<<6));
    GPIOA->MODER |=  (2<<4)|(2<<6);

    GPIOA->AFR[0] &= ~((0xF<<8)|(0xF<<12));
    GPIOA->AFR[0] |=  (7<<8)|(7<<12);
}

/* ----------- UART2 Init ----------- */
void uart2_init(void)
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    USART2->CR1 = 0;
    USART2->BRR = SYSCLK_HZ / 115200;     // CORRECT NOW
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;

    while(!(USART2->ISR & USART_ISR_TEACK));
}

void uart2_putc(char c)
{
    while(!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = c;
}

void uart2_print(char *s)
{
    while(*s) uart2_putc(*s++);
}

/* ----------- DWT microsecond functions ----------- */
void dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t micros(void)
{
    return (DWT->CYCCNT / (SYSCLK_HZ/1000000));
}

/* ----------- Ultrasonic GPIO: PA6=TRIG, PA7=ECHO ----------- */
void ultrasonic_gpio_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // TRIG PA6 OUTPUT
    GPIOA->MODER &= ~(3 << (6*2));
    GPIOA->MODER |=  (1 << (6*2));

    // ECHO PA7 INPUT
    GPIOA->MODER &= ~(3 << (7*2));
}

void trigger_pulse(void)
{
    GPIOA->ODR &= ~(1<<6);

    for(volatile int i=0;i<10;i++);

    GPIOA->ODR |= (1<<6);

    uint32_t start = DWT->CYCCNT;
    uint32_t wait = (SYSCLK_HZ/1000000) * 10;
    while((DWT->CYCCNT - start) < wait);

    GPIOA->ODR &= ~(1<<6);
}

int32_t measure_echo_us(void)
{
    uint32_t timeout = micros() + 30000;

    while(!(GPIOA->IDR & (1<<7)))
    {
        if(micros() > timeout) return -1;
    }

    uint32_t start = micros();

    while(GPIOA->IDR & (1<<7))
    {
        if(micros() - start > 30000) return -2;
    }

    return (micros() - start);
}

void itoa_dec(uint32_t v, char *buf)
{
    char tmp[10];
    int p=0;
    if(v==0){buf[0]='0';buf[1]=0;return;}
    while(v){ tmp[p++] = (v%10)+'0'; v/=10; }
    for(int i=0;i<p;i++) buf[i] = tmp[p-1-i];
    buf[p] = 0;
}

int main(void)
{
    Clock_HSI_Enable();        // **** VERY IMPORTANT ****
    uart2_gpio_init();
    uart2_init();
    dwt_init();
    ultrasonic_gpio_init();

    uart2_print("Starting Ultrasonic...\r\n");

    char buf[16];

    while(1)
    {
        trigger_pulse();
        int32_t us = measure_echo_us();

        if(us > 0)
        {
            itoa_dec(us, buf);
            uart2_print(buf);
            uart2_print(" us\r\n");
        }
        else
        {
            uart2_print("Timeout\r\n");
        }

        for(volatile int i=0;i<200000;i++);
    }
}
