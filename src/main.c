#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "main.h"
#include "dma.h"
#include "usart.h"

#include "config.h"
#include "tim.h"
#include "gpio.h"
//---
#include "code.h"
#include "motor.h"
//#include "laser80M.h"

void SystemClock_Config(void);

int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();

  MX_DMA_Init();
  MX_USART1_UART_Init();

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

  initMotor();          // Начальная инициализация и настройка шаговых моторов
  setSpeedMotor(SPEED); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
                        // testMotorRun();
                        // setZeroMotor(); // Установка в ноль

  // uint8_t UART1_rxBuffer[5] = {0xFA, 0x04, 0x01, 0x80, 0x81}; // Single measurement (broadcast) 
  uint8_t UART1_rxBuffer[5] = {0xFA, 0x04, 0x09, 0x1E, 0xDB}; //    FA 04 09 1E DB 30 м
  SendDataDMA(UART1_rxBuffer, sizeof(UART1_rxBuffer));        // Отправляем данные по указанному UART
  HAL_Delay(200);

  uint8_t UART1_rxBuffer1[5] = {0xFA, 0x04, 0x0C, 0x02, 0xF4}; //    Установить Разрешение: FA 04 0C 02 F4 0,1 мм
  SendDataDMA(UART1_rxBuffer1, sizeof(UART1_rxBuffer1));        // Отправляем данные по указанному UART
  HAL_Delay(200);

  uint8_t UART1_rxBuffer2[5] = {0xFA, 0x04, 0x0A, 0x14, 0xE4}; //  FA 04 0A 0A EE 10Hz - 8Hz real 125 миллисекунд FA 04 0A 14 E4 20Hz - 18Hz real 55 миллисекунд
  SendDataDMA(UART1_rxBuffer2, sizeof(UART1_rxBuffer2));        // Отправляем данные по указанному UART
  HAL_Delay(200);

  uint8_t UART1_rxBuffer3[4] = {0x80, 0x06, 0x03, 0x77}; //  80 06 03 77
  SendDataDMA(UART1_rxBuffer3, sizeof(UART1_rxBuffer3));        // Отправляем данные по указанному UART
  HAL_Delay(5000);

  uint8_t UART1_rxBuffer4[4] = {0x80, 0x04, 0x02, 0x7A}; //  Остановить измерение/ выключения:(ADDR 04 02 CS) 80 04 02 7A
  SendDataDMA(UART1_rxBuffer4, sizeof(UART1_rxBuffer4));        // Отправляем данные по указанному UART

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
