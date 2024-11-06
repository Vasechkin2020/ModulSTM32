#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define RX_BUFFER_SIZE 32                     // Размер буфера приема
uint8_t rx_bufferUART1[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART2[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART3[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART4[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных

#include "main.h"
#include "dma.h"
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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  // uint32_t timeStart = HAL_GetTick(); // Запоминаем время старта

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();

  MX_DMA_Init();

  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();

  MX_SPI1_Init();

  HAL_TIM_Base_Start_IT(&htim6); // Таймер для общего цикла
  HAL_TIM_Base_Start_IT(&htim7); // Таймер для моторов шаговых для датчиков

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, 11); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, 11); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, 11); // Двнные оказываются в буфере rx_bufferUART1

  laserInit();                                                           // Инициализация лазеров
               // Это делаю что-бы нормально работало, а то похоже буфер сбивается и фигня выходит
  HAL_UART_DMAStop(&huart1); // Остановка DMA
  HAL_UART_DMAStop(&huart2); // Остановка DMA
  HAL_UART_DMAStop(&huart3); // Остановка DMA
  HAL_UART_DMAStop(&huart4); // Остановка DMA

  memset(rx_bufferUART1, 0, RX_BUFFER_SIZE); // Очистка буфера
  memset(rx_bufferUART2, 0, RX_BUFFER_SIZE); // Очистка буфера
  memset(rx_bufferUART3, 0, RX_BUFFER_SIZE); // Очистка буфера
  memset(rx_bufferUART4, 0, RX_BUFFER_SIZE); // Очистка буфера

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, 11); // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, 11); // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, 11); // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA

  //  // Запуск обмена данными по SPI с использованием DMA
  initSPI_slave(); // Закладываем начальноы значения и инициализируем буфер DMA
  // HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, BUFFER_SIZE);
  printf("START !!!!!!!!!!!!!!!!!!!!!!!!!!! \r\n");

  initMotor(); // Начальная инициализация и настройка шаговых моторов
  //  testMotorRun();
  setZeroMotor(); // Установка в ноль

  // int a = 0;
  // int b = 2;
  // int c = 0;
  // float aaa = 3.1415255;
  // uint8_t MSG[35] = {'\0'};
  // uint8_t X = 0;

  HAL_Delay(999);
  timeSpi = millis(); // Запоминаем время начала цикла

  while (1)
  {
    loop();

    // a++;
    // b = a * 2;
    // c = a + b;
    // printf("test3 %i %i %i %.2f \n", a, b, c, aaa);
    // printf("Тест %.4f \n", aaa);

    // float my_float = 1.23456;
    // char buffer[20];
    // sprintf(buffer, "%.3f", my_float);
    // printf("Мой поплавок в виде строки: %s\n", buffer);

    // int i = 132;
    // printf("Result is: %d.%d \n", i / 10, i % 10);

    // sprintf(MSG, "Hello VASI! Tracing X = %d\r\n", X);
    // HAL_UART_Transmit(&huart4, MSG, sizeof(MSG), 100);
    // HAL_Delay(500);

    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
    // HAL_Delay(500); // Пауза 500 миллисекунд.
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
// int __io_putchar(int ch)
// {
//   HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
//   return ch;
// }

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
