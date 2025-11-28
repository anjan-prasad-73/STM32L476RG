#include "main.h"
#include <string.h>
#include <stdio.h>

/* ---- Pin Defines ---- */
#define TRIG_PORT GPIOA
#define TRIG_PIN  GPIO_PIN_1
#define ECHO_PORT GPIOA
#define ECHO_PIN  GPIO_PIN_0

#define USER_BUTTON_PIN GPIO_PIN_13

/* ---- Flash Logging Location ---- */
#define FLASH_LOG_START_ADDR 0x080E0000    // Sector 11 start
#define FLASH_SECTOR         FLASH_SECTOR_11

/* ---- Globals ---- */
uint32_t flash_write_ptr = FLASH_LOG_START_ADDR;

/* ---- Function Prototypes ---- */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

void trigger_ultrasonic(void);
uint32_t measure_pulse_us(void);
float measure_distance_cm(void);
int flash_append(const char *text);

/* ---- Main ---- */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    while (1)
    {
        if (HAL_GPIO_ReadPin(GPIOC, USER_BUTTON_PIN) == GPIO_PIN_RESET)
        {
            HAL_Delay(150); // debounce

            float dist = measure_distance_cm();
            char buf[50];

            if (dist < 0)
                sprintf(buf, "ERROR\r\n");
            else
                sprintf(buf, "Distance = %.2f cm\r\n", dist);

            flash_append(buf);

            // Put breakpoint here to inspect 'buf'
            __NOP();
        }

        HAL_Delay(50);
    }
}

/* ---- Create 10µs trigger pulse ---- */
void trigger_ultrasonic(void)
{
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
    HAL_Delay(2);

    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET);
    for (volatile int i = 0; i < 120; i++);   // ~10µs delay
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);
}

/* ---- Measure echo pulse width using polling ---- */
uint32_t measure_pulse_us(void)
{
    uint32_t timeout = 30000;  // max wait

    // Wait for ECHO to go HIGH
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_RESET)
    {
        if (timeout-- == 0) return 0xFFFFFFFF;
    }

    uint32_t start = DWT->CYCCNT;

    // Wait for ECHO to go LOW
    timeout = 30000;
    while (HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN) == GPIO_PIN_SET)
    {
        if (timeout-- == 0) return 0xFFFFFFFF;
    }

    uint32_t end = DWT->CYCCNT;
    uint32_t cycles = end - start;

    return cycles / (SystemCoreClock / 1000000);
}

/* ---- Compute distance ---- */
float measure_distance_cm(void)
{
    trigger_ultrasonic();
    uint32_t us = measure_pulse_us();

    if (us == 0xFFFFFFFF) return -1;
    return (us * 0.0343f) / 2.0f;
}

/* ---- Flash log append ---- */
int flash_append(const char *text)
{
    uint32_t len = strlen(text);
    uint32_t addr = flash_write_ptr;

    if (flash_write_ptr == FLASH_LOG_START_ADDR)
    {
        FLASH_EraseInitTypeDef Erase;
        uint32_t error;

        Erase.TypeErase = FLASH_TYPEERASE_SECTORS;
        Erase.Sector = FLASH_SECTOR;
        Erase.NbSectors = 1;
        Erase.VoltageRange = FLASH_VOLTAGE_RANGE_3;

        HAL_FLASH_Unlock();
        if (HAL_FLASHEx_Erase(&Erase, &error) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return -1;
        }
        HAL_FLASH_Lock();
    }

    HAL_FLASH_Unlock();

    for (uint32_t i = 0; i < len; i += 4)
    {
        uint32_t word = 0xFFFFFFFF;
        memcpy(&word, text + i, (len - i >= 4) ? 4 : len - i);

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, word) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return -1;
        }

        addr += 4;
    }

    HAL_FLASH_Lock();
    flash_write_ptr = addr;
    return 0;
}

/* ---- GPIO Init ---- */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // TRIG (PA1)
    GPIO_InitStruct.Pin = TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

    // ECHO (PA0)
    GPIO_InitStruct.Pin = ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

    // Button (PC13)
    GPIO_InitStruct.Pin = USER_BUTTON_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Enable DWT cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the CPU, AHB and APB busses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    /* 8 MHz HSE → PLL → 168 MHz SYSCLK */
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;     // 168 MHz
    RCC_OscInitStruct.PLL.PLLQ = 7;                 // For USB/SDIO RNG

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks */
    RCC_ClkInitStruct.ClockType =
            RCC_CLOCKTYPE_HCLK  |
            RCC_CLOCKTYPE_SYSCLK |
            RCC_CLOCKTYPE_PCLK1 |
            RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;   // 168 MHz AHB
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;    // 42 MHz APB1 (TIM2–TIM5 run at 84 MHz)
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;    // 84 MHz APB2

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}
void Error_Handler(void)
{
    __disable_irq();  // stop interrupts
    while (1)
    {
        // Optional: Blink LED here for debugging
    }
}
