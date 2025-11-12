#include "main.h"
#include <string.h>

DMA_HandleTypeDef hdma_memtomem;
UART_HandleTypeDef huart2;
SPI_HandleTypeDef hspi2;
uint8_t src_data[] = "MEM_TO_MEM_DMA";
uint8_t dst_data[20];

void SystemClock_Config(void);
void MX_DMA_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_DMA_Init();

  HAL_DMA_Start(&hdma_memtomem, (uint32_t)src_data, (uint32_t)dst_data, strlen((char*)src_data));
  HAL_DMA_PollForTransfer(&hdma_memtomem, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

  while (1);
}

void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();

  hdma_memtomem.Instance = DMA1_Channel1;
  hdma_memtomem.Init.Direction = DMA_MEMORY_TO_MEMORY;
  hdma_memtomem.Init.PeriphInc = DMA_PINC_ENABLE;
  hdma_memtomem.Init.MemInc = DMA_MINC_ENABLE;
  hdma_memtomem.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_memtomem.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_memtomem.Init.Mode = DMA_NORMAL;
  hdma_memtomem.Init.Priority = DMA_PRIORITY_LOW;
  HAL_DMA_Init(&hdma_memtomem);
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}

