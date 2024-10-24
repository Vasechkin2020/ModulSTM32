#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define RX_BUFFER_SIZE 64                     // Размер буфера приема
uint8_t rx_bufferUART1[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART2[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART3[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных
uint8_t rx_bufferUART4[RX_BUFFER_SIZE] = {0}; // Буфер для приема данных

#include "main.h"
#include "dma.h"
#include "usart.h"

#include "config.h"
#include "tim.h"
#include "gpio.h"
//---
#include "code.h"
#include "motor.h"
#include "laser80M.h"

void SystemClock_Config(void);

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

  HAL_TIM_Base_Start_IT(&htim6); // Таймер для общего цикла
  HAL_TIM_Base_Start_IT(&htim7); // Таймер для моторов шаговых для датчиков

  // initMotor();          // Начальная инициализация и настройка шаговых моторов
  // setSpeedMotor(SPEED); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
  // testMotorRun();
  // setZeroMotor(); // Установка в ноль

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_bufferUART2, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_bufferUART3, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
  HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_bufferUART4, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1

  laser80_setAddress(huart1, 0x80);
  laser80_setAddress(huart2, 0x80);
  laser80_setAddress(huart3, 0x80);
  laser80_setAddress(huart4, 0x80);
  HAL_Delay(100);
  laser80_stopMeasurement(huart1, 0x80);
  laser80_stopMeasurement(huart2, 0x80);
  laser80_stopMeasurement(huart3, 0x80);
  laser80_stopMeasurement(huart4, 0x80);
  HAL_Delay(100);
  laser80_controlLaser(huart1, 1, 0x80);
  laser80_controlLaser(huart2, 1, 0x80);
  laser80_controlLaser(huart3, 1, 0x80);
  laser80_controlLaser(huart4, 1, 0x80);
  HAL_Delay(2000);
  laser80_controlLaser(huart1, 0, 0x80);
  laser80_controlLaser(huart2, 0, 0x80);
  laser80_controlLaser(huart3, 0, 0x80);
  laser80_controlLaser(huart4, 0, 0x80);
  HAL_Delay(100);

  // laser80_setTimeInterval(huart1,0);
  // laser80_setResolution(huart1,1);
  // laser80_setRange(huart1,30);
  // laser80_setStartingPoint(huart1,1);
  laser80_setFrequency(huart1, 3);
  laser80_setFrequency(huart2, 3);
  laser80_setFrequency(huart3, 3);
  laser80_setFrequency(huart4, 3);

  // Это делаю что-бы нормально работало, а то похоже буфер сбивается и фигня выходит
  HAL_UART_DMAStop(&huart1);                                             // Остановка DMA
  HAL_UART_DMAStop(&huart2);                                             // Остановка DMA
  HAL_UART_DMAStop(&huart3);                                             // Остановка DMA
  HAL_UART_DMAStop(&huart4);                                             // Остановка DMA

  memset(rx_bufferUART1, 0, RX_BUFFER_SIZE);                             // Очистка буфера
  memset(rx_bufferUART2, 0, RX_BUFFER_SIZE);                             // Очистка буфера
  memset(rx_bufferUART3, 0, RX_BUFFER_SIZE);                             // Очистка буфера
  memset(rx_bufferUART4, 0, RX_BUFFER_SIZE);                             // Очистка буфера
  
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_bufferUART2, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_bufferUART3, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
  HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_bufferUART4, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA

  // Непрерывное измерение
  laser80_continuousMeasurement(huart1,0x80); // Данные пойдут только через 500 милисекунд
  laser80_continuousMeasurement(huart2,0x80); // Данные пойдут только через 500 милисекунд
  laser80_continuousMeasurement(huart3,0x80); // Данные пойдут только через 500 милисекунд
  laser80_continuousMeasurement(huart4,0x80); // Данные пойдут только через 500 милисекунд

  // HAL_Delay(5000);
  // laser80_stopMeasurement(huart1,0x80);

  // int a = 0;
  // int b = 2;
  // int c = 0;
  // float aaa = 3.1415255;
  // uint8_t MSG[35] = {'\0'};
  // uint8_t X = 0;

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
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart4, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

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
