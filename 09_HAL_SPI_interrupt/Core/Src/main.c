#include "main.h"
#include <string.h>

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart2_rx;

#define BUFFER_SIZE 32

uint8_t txBuffer[BUFFER_SIZE] = "UART TxToRx DMA Test\r\n";
uint8_t rxBuffer[BUFFER_SIZE];
volatile uint8_t dmaDone = 0;
volatile uint8_t dataMatched = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    dmaDone = 1;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    dmaDone = 1;
}

void HAL_UART_TxToRxCopy_DMA(void)
{
    /* Clear receive buffer */
    memset(rxBuffer, 0, BUFFER_SIZE);

    /* Start DMA Receive first */
    HAL_UART_Receive_DMA(&huart2, rxBuffer, strlen((char*)txBuffer));

    /* Then start DMA Transmit */
    HAL_UART_Transmit_DMA(&huart2, txBuffer, strlen((char*)txBuffer));
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART2_UART_Init();

    HAL_UART_TxToRxCopy_DMA();

    while (1)
    {
        if (dmaDone)
        {
            dmaDone = 0;
            if (memcmp(txBuffer, rxBuffer, strlen((char*)txBuffer)) == 0)
                dataMatched = 1;
            else
                dataMatched = 0;

            HAL_Delay(1000);
            HAL_UART_TxToRxCopy_DMA(); // repeat every 1 sec
        }
    }
}

/* DMA + UART + Clock init (same as before) */
static void MX_USART2_UART_Init(void)
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

static void MX_DMA_Init(void)
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

void DMA1_Channel6_IRQHandler(void) { HAL_DMA_IRQHandler(huart2.hdmarx); }
void DMA1_Channel7_IRQHandler(void) { HAL_DMA_IRQHandler(huart2.hdmatx); }

static void MX_GPIO_Init(void) { __HAL_RCC_GPIOA_CLK_ENABLE(); }

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLR = 2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}
