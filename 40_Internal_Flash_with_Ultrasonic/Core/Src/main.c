#include "main.h"
#include <stdio.h>
#include<string.h>

UART_HandleTypeDef huart2;
TIM_HandleTypeDef htim2;

uint32_t IC_Val1, IC_Val2;
uint8_t Is_First_Captured = 0;
uint32_t Distance;

#define FLASH_ADDR 0x080E0000

void delay(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2,0);
    while(__HAL_TIM_GET_COUNTER(&htim2) < us);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if(!Is_First_Captured)
        {
            IC_Val1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
            Is_First_Captured = 1;
        }
        else
        {
            IC_Val2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            uint32_t diff = IC_Val2-IC_Val1;

            Distance = diff*0.034/2;
            Is_First_Captured = 0;

            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

            HAL_TIM_IC_Stop_IT(&htim2,TIM_CHANNEL_1);
        }
    }
}

void Flash_Write(uint32_t data)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;

    EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = 255;     // (0x080E0000)
    EraseInitStruct.NbPages     = 1;

    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);

    uint64_t d = data;
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, FLASH_ADDR, d);

    HAL_FLASH_Lock();
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_TIM2_Init();

    HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);

    while(1)
    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
        delay(10);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);

        HAL_Delay(100);

        Flash_Write(Distance);

        char msg[50];
        sprintf(msg,"Distance = %lu cm\r\n",Distance);
        HAL_UART_Transmit(&huart2,(uint8_t*)msg,strlen(msg),100);
    }
}
