
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

// ВЫБОР С КАКИМИ ДАТЧИКАМИ РАБОТАЕМ. НУЖНО ОСТАВИТЬТОЛЬКО ОДНУ СРОЧКУ, ОСТАЛЬНЫЕ ЗАКОММЕНТИРОВАТЬ
#define LASER80 yes
// #define LASER60 yes
// #define LASER50 yes

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

    struct dataUART
    {
        uint8_t flag;      // Флаг готовности данных
        uint8_t num;       // Номер UART
        uint32_t status;   // Статус данных
        uint8_t statusDMA; // Статус вызова нового DMA
        uint32_t distance; // Дистанция по последнему хорошему измерению
        uint16_t quality;  // Качество сигнала
        float angle;       // Угол в котором находился мотор в момент когда пришли данные по измерению
        uint32_t time;     // Время измерения от начала запуска программы
        uint32_t timeSend;     // Время отправки запроса к датчику
        uint8_t *adr;      // Адрес буфера
        uint16_t len;       // Длинна данных в буфере
        UART_HandleTypeDef *huart;
    };

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
