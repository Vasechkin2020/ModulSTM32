
#include "main.h"
#include "stm32g0xx_it.h"
#include "config.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_rx;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_usart4_rx;

extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

extern DMA_HandleTypeDef hdma_i2c1_rx;

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**  * @brief This function handles Non maskable interrupt.  */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/**   * @brief This function handles Hard fault interrupt.  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**  * @brief This function handles System service call via SWI instruction.  */
void SVC_Handler(void)
{
}

/**   * @brief This function handles Pendable request for system service.   */
void PendSV_Handler(void)
{
}

/**   * @brief This function handles System tick timer.  */
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/**  * @brief This function handles TIM6 global interrupt.  */
void TIM6_IRQHandler(void) // Функция, которая обрабатывает прерывание на уровне HAL. Она сбрасывает флаг прерывания и вызывает пользовательский callback.
{
  HAL_TIM_IRQHandler(&htim6);
  // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
}
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}

// * @brief This function handles EXTI line 0 and line 1 interrupts.
void EXTI0_1_IRQHandler(void) // Обработчик прерывания EXTI для кнопки
{
  HAL_GPIO_EXTI_IRQHandler(micMotor1_Pin);
}

/**  * @brief This function handles EXTI line 2 and line 3 interrupts.  */
void EXTI2_3_IRQHandler(void) // Обработчик прерывания EXTI для кнопки
{
  HAL_GPIO_EXTI_IRQHandler(micMotor0_Pin);
}

/**  * @brief This function handles EXTI line 4 to 15 interrupts.  */
void EXTI4_15_IRQHandler(void) // Обработчик прерывания EXTI для кнопки
{
  HAL_GPIO_EXTI_IRQHandler(micMotor2_Pin);
  HAL_GPIO_EXTI_IRQHandler(micMotor3_Pin);
}

/**   * @brief This function handles DMA1 channel 2 and channel 3 interrupts.   */
void DMA1_Channel2_3_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart1_rx);
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

/**   * @brief This function handles DMA1 channel 4, channel 5, channel 6, channel 7 and DMAMUX1 interrupts.  */
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  HAL_DMA_IRQHandler(&hdma_usart4_rx);
  HAL_DMA_IRQHandler(&hdma_spi1_tx); //и передающего каналов
  HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

/** * @brief This function handles USART1 global interrupt  */
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

/**   * @brief This function handles USART2 global interrupt  */
void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}
/**   * @brief This function handles USART3 and USART4 interrupts.*/
void USART3_4_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart3);
  HAL_UART_IRQHandler(&huart4);
}


/**  * @brief This function handles DMA1 channel 1 interrupt.  */ // Прерывания DMA для приемного 
void DMA1_Channel1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
}

