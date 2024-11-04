/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SLAVE_SPI_H__
#define __SLAVE_SPI_H__

#include "main.h"
#include "dma.h"
#include "spi.h"

#include "config.h"
#include "slaveSPI.h"

u_int64_t timeSpi = 0; // Время когда пришла команда по SPI

extern SPI_HandleTypeDef hspi1;
volatile bool flag_data = false; // Флаг что данные передались

#define BUFFER_SIZE 168 // Размер буфера который передаем. Следить что-бы структуры не превышали этот размер Кратно 32 делать
// uint8_t txBuffer[BUFFER_SIZE] = {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xA0}; // = "Hello from STM32 Slave"; // Передающий буфер
uint8_t txBuffer[BUFFER_SIZE] = {0}; // = "Hello from STM32 Slave"; // Передающий буфер
uint8_t rxBuffer[BUFFER_SIZE];       // Принимающий буфер

volatile uint8_t transferComplete = 0; // Флаг завершения передачи

//********************** ОБЯВЛЕНИЕ ФУНКЦИЙ ================================

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi); // Обработчик прерывания при завершении обмена данных по DMA
void initSPI_slave();                                   // Начальная инициализция для SPI
void spi_slave_queue_Send();                            // Функция в которой чистим буфер и закладываем данные на передачу в буфер
void processingDataReceive();                           // Обработка по флагу в main пришедших данных после срабатывания прерывания что обмен состоялся

//********************** РЕАЛИЗАЦИЯ ФУНКЦИЙ ================================

// Функция возвращает контрольную сумму структуры без последних 4 байтов
uint32_t measureCheksum_Data2Modul(const struct Struct_Data2Modul *structura_)
{
    uint32_t ret = 0;
    unsigned char *adr_structura = (unsigned char *)(structura_); // Запоминаем адрес начала структуры. Используем для побайтной передачи
    for (int i = 0; i < sizeof(structura_) - 4; i++)
    {
        ret += adr_structura[i]; // Побайтно складываем все байты структуры кроме последних 4 в которых переменная в которую запишем результат
    }
    return ret;
}
// Функция возвращает контрольную сумму структуры без последних 4 байтов
uint32_t measureCheksum_Modul2Data(const struct Struct_Modul2Data *structura_)
{
    uint32_t ret = 0;
    unsigned char *adr_structura = (unsigned char *)(structura_); // Запоминаем адрес начала структуры. Используем для побайтной передачи
    for (int i = 0; i < sizeof(structura_) - 4; i++)
    {
        ret += adr_structura[i]; // Побайтно складываем все байты структуры кроме последних 4 в которых переменная в которую запишем результат
    }
    return ret;
}

// Обработчик прерывания при завершении обмена данных по DMA
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi1)
    {
        HAL_GPIO_WritePin(Analiz1_GPIO_Port, Analiz1_Pin, GPIO_PIN_SET);   // Инвертирование состояния выхода.
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);                      // Инвертирование состояния выхода.
        HAL_GPIO_WritePin(Analiz1_GPIO_Port, Analiz1_Pin, GPIO_PIN_RESET); // Инвертирование состояния выхода.

        //  Обработка полученных данных из rxBuffer
        // transferComplete = 1; // Устанавливаем флаг завершения обмена
        flag_data = true; // Флаг что обменялись данными
        spi.all++;        // Считаем сколько было обменов данными всего

        // // Перезапуск функции для следующего обмена
        // HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, BUFFER_SIZE); // Запуск обмена данными по SPI с использованием DMA
        // if (HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, BUFFER_SIZE) != HAL_OK)
        // {
        //     Error_Handler(); // Обработка ошибки при повторном запуске
        // }
    }
}

extern void collect_Data_for_Send();

// Начальная инициализция для SPI
void initSPI_slave()
{
    collect_Data_for_Send(); // Собираем данные для начальной отправки
    spi_slave_queue_Send();  // Начальная закладка данных в буфер
    // const uint16_t size_structura_receive = sizeof(Data2Modul_receive); // Размер структуры с данными которые получаем
    // const uint16_t size_structura_send = sizeof(Modul2Data_send);       // Размер структуры с данными которые передаем
    // uint16_t max_size_stuct;
    // max_size_stuct = 0;
    // if (size_structura_receive < size_structura_send)
    //     max_size_stuct = size_structura_send;
    // else
    //     max_size_stuct = size_structura_receive; // Какая из структур больше
}

// Функция в которой чистим буфер и закладываем данные на передачу в буфер
void spi_slave_queue_Send()
{
    memset(txBuffer, 0, sizeof(txBuffer));
    memset(rxBuffer, 0, sizeof(rxBuffer));
    struct Struct_Modul2Data *copy_txBuffer = (struct Struct_Modul2Data *)txBuffer; // Создаем переменную в которую пишем адрес буфера в нужном формате
    *copy_txBuffer = Modul2Data_send;                                               // Копируем из структуры данные в пвмять начиная с адреса в котором начинаяется буфер для передачи

    HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, BUFFER_SIZE); // Запуск обмена данными по SPI с использованием DMA //Перезапуск функции для следующего обмена
}

// Обработка по флагу в main пришедших данных после срабатывания прерывания что обмен состоялся
void processingDataReceive()
{
    // struct STest
    // {
    //     uint8_t byte0;
    //     uint8_t byte1;
    //     uint8_t byte2;
    //     uint8_t byte3;
    //     float fff;
    // };

    // struct STest StructTestPSpi;

    // struct STest StructTestPSpi_temp;                       // Экземпляр структуры получаемых данных временный, пока не посчитаем контроьную сумму и убедимся что данные хорошие
    // struct STest *copy_rxBuffer = (struct STest *)rxBuffer; // Создаем переменную в которую пишем адрес буфера в нужном формате
    // StructTestPSpi_temp = *copy_rxBuffer;                   // Копируем из этой перемнной данные в мою структуру

    struct Struct_Data2Modul Data2Modul_receive_temp;                               // Экземпляр структуры получаемых данных временный, пока не посчитаем контроьную сумму и убедимся что данные хорошие
    struct Struct_Data2Modul *copy_rxBuffer = (struct Struct_Data2Modul *)rxBuffer; // Создаем переменную в которую пишем адрес буфера в нужном формате
    Data2Modul_receive_temp = *copy_rxBuffer;                                       // Копируем из этой перемнной данные в мою структуру

    uint32_t cheksum_receive = 0; // = measureCheksum(Data2Modul_receive_temp);             // Считаем контрольную сумму пришедшей структуры

    // unsigned char *adr_structura = (unsigned char *)(&StructTestPSpi_temp); // Запоминаем адрес начала структуры. Используем для побайтной передачи

    unsigned char *adr_structura = (unsigned char *)(&Data2Modul_receive_temp); // Запоминаем адрес начала структуры. Используем для побайтной передачи
    for (int i = 0; i < sizeof(Data2Modul_receive_temp) - 4; i++)
    {
        cheksum_receive += adr_structura[i]; // Побайтно складываем все байты структуры кроме последних 4 в которых переменная в которую запишем результат
    }

    if (cheksum_receive != Data2Modul_receive_temp.cheksum || Data2Modul_receive_temp.cheksum == 0) // Стравниваю что сам посчитал и что прислали. Не сходится или ноль - значит плохие данные
    {
        spi.bed++; // Плохие данные
    }
    else
    {
        Data2Modul_receive = Data2Modul_receive_temp; // Хорошие данные копируем
        // printf("Data OK. /r/n");
    }
    // printf("b1 = %#X b2 = %#X b3 = %#X b4 = %#X %.4f = ", StructTestPSpi_temp.byte0, StructTestPSpi_temp.byte1, StructTestPSpi_temp.byte2, StructTestPSpi_temp.byte3, StructTestPSpi_temp.fff);
    //  for (int i = 0; i < sizeof(Data2Modul_receive); i++)
    //  {
    //      printf("%#X ", adr_structura[i]);
    //  }
}
#endif /* __SPI_H__ */
