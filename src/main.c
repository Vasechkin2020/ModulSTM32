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

  HAL_TIM_Base_Start_IT(&htim6); // Таймер для общего цикла
  HAL_TIM_Base_Start_IT(&htim7); // Таймер для моторов шаговых для датчиков

  initMotor();          // Начальная инициализация и настройка шаговых моторов
  setSpeedMotor(SPEED); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
                        // testMotorRun();
                        // setZeroMotor(); // Установка в ноль

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1

  laser80_setAddress(0x80);
  laser80_stopMeasurement(0x80);
  HAL_Delay(500);
  // laser80_controlLaser(1, 0x80);
  // HAL_Delay(200);
  // laser80_controlLaser(0, 0x80);
  // laser80_setTimeInterval(0);
  // laser80_setResolution(1);
  // laser80_setRange(30);
  // laser80_setStartingPoint(1);
  // laser80_setFrequency(10);

  // Это делаю что-бы нормально работало, а то похоже буфер сбивается и фигня выходит
  HAL_UART_DMAStop(&huart1);  // Остановка DMA
  memset(rx_bufferUART1, 0, RX_BUFFER_SIZE);  // Очистка буфера
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA

  // Непрерывное измерение
  // laser80_continuousMeasurement(0x80); // Данные пойдут только через 500 милисекунд

  // HAL_Delay(5000);
  // laser80_stopMeasurement(0x80);

  while (1)
  {
    loop();

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
