/* q2_env_i2c.c
   Read temp/humidity via I2C1 every 1s, LED blink if temp>35C
*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

#define LED_PORT GPIOA
#define LED_PIN  GPIO_PIN_5

I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  uint8_t cmd[2] = {0x24, 0x00}; // single shot high repeatability (SHT31)
  uint8_t data[6];
  float temp, hum;

  while (1)
  {
    if (HAL_I2C_Master_Transmit(&hi2c1, (0x44<<1), cmd, 2, 100) == HAL_OK)
    {
      HAL_Delay(20);
      if (HAL_I2C_Master_Receive(&hi2c1, (0x44<<1), data, 6, 100) == HAL_OK)
      {
        uint16_t t_raw = (data[0]<<8) | data[1];
        uint16_t h_raw = (data[3]<<8) | data[4];
        temp = -45.0f + 175.0f * ( (float)t_raw / 65535.0f );
        hum  = 100.0f * ( (float)h_raw / 65535.0f );

        if (temp > 35.0f) HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
      }
    }
    HAL_Delay(1000);
  }
}

/* I2C1 init */
static void MX_I2C1_Init(void)
{
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00707CBB;         // 100 kHz timing
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLED) != HAL_OK)
    {
        Error_Handler();
    }
}


/* UART init (same as others) */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  if (HAL_UART_Init(&huart2) != HAL_OK) Error_Handler();
}

/* GPIO init (I2C pins assumed in Cube config; here we set LED + button if needed) */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // LED PA5
  GPIO_InitStruct.Pin = LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PORT, &GPIO_InitStruct);

  // Button PC13
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/* SystemClock_Config & Error_Handler same as previous (copy/paste) */
void SystemClock_Config(void) { /* ...same as file1... */ RCC_OscInitTypeDef RCC_OscInitStruct = {0}; RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) Error_Handler(); RCC_OscInitStruct.OscillatorType = RCC_OSCILLATETYPE_HSI; RCC_OscInitStruct.HSIState = RCC_HSI_ON; RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI; RCC_OscInitStruct.PLL.PLLM = 1; RCC_OscInitStruct.PLL.PLLN = 10; RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7; RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler(); RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2; RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1; if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) Error_Handler(); }
void Error_Handler(void) { __disable_irq(); while (1) {} }
