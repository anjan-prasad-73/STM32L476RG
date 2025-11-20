/* 36_ADCBareMetal.c
   STM32L476RG - ADC1 single conversion on PA0. Prints value over USART2 (polling).
*/

#include "stm32l476xx.h"
#include <stdio.h>

#define SYSCLK_HZ 16000000U

/* Minimal putchar via USART2 polling */
void uart2_gpio_init(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~((3<<4)|(3<<6));
    GPIOA->MODER |= (2<<4)|(2<<6);
    GPIOA->AFR[0] &= ~((0xF<<8)|(0xF<<12));
    GPIOA->AFR[0] |= (7<<8)|(7<<12);
}
void uart2_init(uint32_t pclk){
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
    USART2->CR1 = 0;
    USART2->BRR = (uint32_t)((pclk + (115200/2))/115200);
    USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;
    while(!(USART2->ISR & USART_ISR_TEACK));
}
int putchar_poll(int ch){
    while(!(USART2->ISR & USART_ISR_TXE));
    USART2->TDR = (uint8_t)ch;
    return ch;
}

/* ADC on PA0 (ADC_IN5 typical mapping for L476 packages - check your package if in doubt) */
void adc_gpio_init(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER |= (3 << (0*2)); // PA0 analog
}

void adc_init(void){
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;
    /* Ensure ADEN = 0 */
    ADC1->CR &= ~ADC_CR_ADEN;
    /* Calibrate */
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);
    /* Single conversion, right align, 12-bit default */
    ADC1->SQR1 = 0; // regular sequence length = 1
    ADC1->SQR1 |= (5 << 6); // first conversion = channel 5 (PA0 -> IN5 in many packages)
    /* Sampling time for channel in SMPR1 (channel5 uses SMPR1) */
    ADC1->SMPR1 |= (3 << (3*5)); // sample time; moderate
    /* Enable ADC */
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
}

uint16_t adc_read_once(void){
    ADC1->CR |= ADC_CR_ADSTART;
    while(!(ADC1->ISR & ADC_ISR_EOC));
    return (uint16_t)ADC1->DR;
}

void itoa_dec(uint32_t val, char *buf){
    char tmp[12];
    int i=0;
    if(val==0){ buf[0]='0'; buf[1]=0; return;}
    while(val){ tmp[i++] = '0' + (val%10); val/=10;}
    for(int j=0;j<i;j++) buf[j]=tmp[i-1-j];
    buf[i]=0;
}

int main(void){
    uart2_gpio_init();
    uart2_init(SYSCLK_HZ);

    adc_gpio_init();
    adc_init();

    while(1){
        uint16_t v = adc_read_once();
        char s[16];
        itoa_dec(v, s);
        for(char *p=s; *p; ++p) putchar_poll(*p);
        putchar_poll('\r'); putchar_poll('\n');
        for (volatile uint32_t d=0; d<200000; ++d) __NOP();
    }
}
