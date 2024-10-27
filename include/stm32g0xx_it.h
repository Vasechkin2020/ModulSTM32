
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

void EXTI0_1_IRQHandler(void);
void EXTI2_3_IRQHandler(void);
void EXTI4_15_IRQHandler(void);

void DMA1_Channel1_IRQHandler(void); //SPI

void DMA1_Channel2_3_IRQHandler(void);
void DMA1_Ch4_7_DMAMUX1_OVR_IRQHandler(void);

void USART1_IRQHandler(void); // Шлобальное прерывание. Не использую, потом отключить

#ifdef __cplusplus
}
#endif

#endif /* __STM32G0xx_IT_H */
