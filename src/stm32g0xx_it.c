
#include "main.h"
#include "stm32g0xx_it.h"

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;

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
void TIM6_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim6);
  // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
}
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}
