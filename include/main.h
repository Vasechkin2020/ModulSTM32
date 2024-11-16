
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Установите DEBUG для включения отладочной информации
#define DEBUG 1 // Поставьте 0 для отключения отладочной информации

#if DEBUG
    #define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...) (void)0; // Приведение 0 к типу void, ничего не делает
#endif


// #include <stdio.h>

// Уровни отладки
#define DEBUG_LEVEL_NONE  0 // Отключение всех сообщений
#define DEBUG_LEVEL_ERROR 1 // Только ошибки
#define DEBUG_LEVEL_WARN  2 // Предупреждения и ошибки
#define DEBUG_LEVEL_INFO  3 // Информационные сообщения, предупреждения и ошибки
#define DEBUG_LEVEL_DEBUG 4 // Полный вывод, включая отладочные сообщения

// Установка текущего уровня отладки
#define DEBUG_LEVEL DEBUG_LEVEL_INFO

// Макросы для отладочного вывода
#if DEBUG_LEVEL >= DEBUG_LEVEL_ERROR
    #define PRINTF1(fmt, ...) printf("[ERROR]: " fmt "\n", ##__VA_ARGS__)
#else
    #define PRINTF1(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_WARN
    #define PRINTF2(fmt, ...) printf("[WARN]: " fmt "\n", ##__VA_ARGS__)
#else
    #define PRINTF2(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_INFO
    #define PRINTF3(fmt, ...) printf("[INFO]: " fmt "\n", ##__VA_ARGS__)
#else
    #define PRINTF3(fmt, ...)
#endif

#if DEBUG_LEVEL >= DEBUG_LEVEL_DEBUG
    #define PRINTF4(fmt, ...) printf("[DEBUG]: " fmt "\n", ##__VA_ARGS__)
#else
    #define PRINTF4(fmt, ...)
#endif

// int main() {
//     PRINTF1("This is an error message.");
//     PRINTF2("This is a warning message.");
//     PRINTF3("This is an info message.");
//     PRINTF4("This is a debug message.");

//     return 0;
// }

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
