
#ifndef __STM32G0xx_IT_H
#define __STM32G0xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void TIM6_IRQHandler(void);
void TIM7_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32G0xx_IT_H */
