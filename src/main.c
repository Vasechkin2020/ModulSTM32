#ifndef MAIN_H
#define MAIN_H
// ver 1.1
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "usart.h"
#include "spi.h"

#include "config.h"
#include "tim.h"
#include "gpio.h"
//---
#include "code.h"
#include "motor.h"
#include "laser80M.h"
#include "slaveSPI.h"

void SystemClock_Config(void);

volatile uint32_t millisCounter = 0;

int main(void)
{
  HAL_Init();           /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  SystemClock_Config(); /* Configure the system clock */

  // uint32_t timeStart = HAL_GetTick(); // Запоминаем время старта

  MX_GPIO_Init(); /* Initialize all configured peripherals */
  MX_TIM6_Init();
  MX_TIM7_Init();

  MX_DMA_Init();

  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();

  MX_SPI1_Init();
  MX_I2C1_Init();

  HAL_TIM_Base_Start_IT(&htim6); // Таймер для общего цикла
  HAL_TIM_Base_Start_IT(&htim7); // Таймер для моторов шаговых для датчиков

  DEBUG_PRINTF("Это ОТЛАДОЧНЫЙ режим вывода \r\n");
  initLaser(); // Инициализация лазеров зависимоти от типа датчкика. определяем переменные буфер приема для каждого UART

  initMotor(); // Начальная инициализация и настройка шаговых моторов
  // setZeroMotor(); // Установка в ноль
  //  testMotorRun();
  
  initSPI_slave(); // Закладываем начальноы значения и инициализируем буфер DMA //  // Запуск обмена данными по SPI с использованием DMA

  // HAL_Delay(999);
  timeSpi = millis(); // Запоминаем время начала цикла
  // DEBUG_PRINTF("%lli LOOP !!!!!!!!!!!!!!!!!!!!!!!!!!! \r\n",timeSpi);

  while (1)
  {
    workingSPI();         // Отработка действий по обмену по шине SPI
    workingLaser();       // Отработка действий по лазерным датчикам
    workingTimer();       // Отработка действий по таймеру в 1, 50, 60 милисекунд
    workingStopTimeOut(); // Остановка драйверов и моторов при обрыве связи
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters in the RCC_OscInitTypeDef structure. */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
// Перенаправление вывода команды printf на UART
#if DEBUG
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
#endif

// Обработчик ошибок
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
    // HAL_Delay(100);
  }
}

// Переопределяем обработчик SysTick, чтобы увеличивать счётчик миллисекунд
// void HAL_SYSTICK_Callback(void)
// {
//   millisCounter++; // Увеличиваем счетчик миллисекунд
// }

// #define SYSTICK_LOAD (SystemCoreClock / 1000000U)
// #define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

// #define DELAY_US(us)
//   do
//   {
//     uint32_t start = SysTick->VAL;
//     uint32_t ticks = (us * SYSTICK_LOAD) - SYSTICK_DELAY_CALIB;
//     while ((start - SysTick->VAL) < ticks)
//       ;
//   } while (0)

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#endif /*MAIN_H*/