#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "laser80M.h"

//********************************* ПЕРЕМЕННЫЕ ***************************************************************************

bool flag_timer_10millisec = false;
bool flag_timer_50millisec = false;
bool flag_timer_1sec = false;

GPIO_TypeDef *myPort;

void loop();
void timer6(); // Обработчик прерывания таймера TIM6	1 раз в 1 милисекунду
float distanceUART1 = 0;
int flagContinius = 0;

//********************************* ФУНКЦИИ ***************************************************************************

void timer6() // Обработчик прерывания таймера TIM6	1 раз в 1 милисекунду
{
    static int count_timer_10millisec = 0; // Счетчик для запуска обработки движения моторов в лупе по флагу
    static int count_timer_50millisec = 0; // Счетчик для запуска каждые 50 милисекунд
    static int count_timer_1sec = 0;       // Счетчик для запуска

    count_timer_10millisec++;
    count_timer_50millisec++;
    count_timer_1sec++;

    //  каждые 10 милисекунд
    if (count_timer_10millisec >= 10)
    {
        count_timer_10millisec = 0;
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);
        flag_timer_10millisec = true;
    }
    // каждые 50 милисекунд
    if (count_timer_50millisec >= 50)
    {
        count_timer_50millisec = 0;
        flag_timer_50millisec = true;
    }
    // 1 seconds
    if (count_timer_1sec >= 1000)
    {
        count_timer_1sec = 0;
        flag_timer_1sec = true;
    }
}
// После настройки UART и DMA, данные можно отправить с помощью функции В этом примере data — указатель на буфер с данными, которые нужно отправить, а size — количество байт для отправки.
// void SendDataDMA(UART_HandleTypeDef huart, uint8_t *data, uint16_t size)
// {
//     // Запускаем передачу данных по DMA
//     if (HAL_UART_Transmit_DMA(&huart, data, size) != HAL_OK)
//     {
//         // Обработка ошибки
//         Error_Handler();
//     }
// }

// Когда DMA завершит передачу, произойдет прерывание, и вызовется функция обратного вызова HAL_UART_TxCpltCallback. В ней можно обработать завершение передачи, например, запустить передачу новых данных или освободить буфер.
// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         // Обработка завершения передачи по UART1 // Например, можно вывести сообщение о том, что передача завершена
//         // HAL_GPIO_TogglePin(Analiz2_GPIO_Port, Analiz2_Pin); // Инвертирование состояния выхода.
//         // HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin);       // Инвертирование состояния выхода.
//     }
// }
// Пример обработки данных
void ProcessReceivedData(uint8_t *data, uint16_t size)
{
    // HAL_GPIO_TogglePin(Analiz1_GPIO_Port, Analiz1_Pin); // Инвертирование состояния выхода.
    // HAL_GPIO_TogglePin(Led1_GPIO_Port, Led1_Pin);       // Инвертирование состояния выхода.
    // Ваш код обработки данных
    // for (uint16_t i = 0; i < size; i++)
    // {
    //     // Например, вывод полученных данных
    //     printf("Received byte: 0x%02X\n", data[i]);
    // }
}

struct dataUART
{
    uint8_t flag;   // Флаг готовности данных
    uint8_t len;    // Длинна полученных данных в буфере
    uint8_t num;    // Номер UART
    float distance; // Дистанция по последнему хорошему измерению
};

struct dataUART dataUART1;
struct dataUART dataUART2;
struct dataUART dataUART3;
struct dataUART dataUART4;

// Коллбэк, вызываемый при событии UART Idle по окончания приема
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        // HAL_GPIO_TogglePin(Analiz1_GPIO_Port, Analiz1_Pin); // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(Analiz1_GPIO_Port, Analiz1_Pin); // Инвертирование состояния выхода.
        // Обработка полученных данных
        dataUART1.flag = 1;
        dataUART1.len = Size;
        dataUART1.num = 1;
        // После обработки вновь запустить прием
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE);

        // ProcessReceivedData(rx_bufferUART1, Size);
        // if (codeOperationUART1 == Stop) // Если ждем ответа на оправленную команду Stop
        // {
        //     if (rx_bufferUART1[0] == 0x80 && rx_bufferUART1[1] == 0x04 && rx_bufferUART1[2] == 0x82 && rx_bufferUART1[3] == 0xFA)
        //     {
        //         // printf("stopMeasurement ok \n");
        //     }
        //     else
        //     {
        //         // printf("stopMeasurement ERROR \n");
        //     }
        // }
        // if (codeOperationUART1 == Continuous) // Если ждем ответа на оправленную команду Continuous
        // {
        //     flagContinius = 1;
        //     //HAL_GPIO_TogglePin(Analiz2_GPIO_Port, Analiz2_Pin); // Инвертирование состояния выхода.
        // }
    }
    if (huart->Instance == USART2)
    {
        dataUART2.flag = 1; // Обработка полученных данных
        dataUART2.len = Size;
        dataUART2.num = 2;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_bufferUART2, RX_BUFFER_SIZE); // После обработки вновь запустить прием
    }
    if (huart->Instance == USART3)
    {
        dataUART3.flag = 1; // Обработка полученных данных
        dataUART3.len = Size;
        dataUART3.num = 3;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_bufferUART3, RX_BUFFER_SIZE); // После обработки вновь запустить прием
    }
    if (huart->Instance == USART4)
    {
        dataUART4.flag = 1; // Обработка полученных данных
        dataUART4.len = Size;
        dataUART4.num = 4;
        HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_bufferUART4, RX_BUFFER_SIZE); // После обработки вновь запустить прием
    }
}
// Вычисление дистанции по полученному буферу или ошибка
float calcDistance(uint8_t *rx_bufferUART, u_int8_t len_)
{
    float distance = 0;
    if (len_ == 11 && rx_bufferUART[3] != 0x45 && rx_bufferUART[4] != 0x52 && rx_bufferUART[5] != 0x52) // по кодам ASCII ERR
    {
        uint8_t sot = (rx_bufferUART[3] - 0x30) * 100;    // По таблице ASCII отнимаем 48 и получаем сколько сотен метров
        uint8_t des = (rx_bufferUART[4] - 0x30) * 10;     // По таблице ASCII отнимаем 48 и получаем сколько десятков метров
        uint8_t met = (rx_bufferUART[5] - 0x30) * 1;      // По таблице ASCII отнимаем 48 и получаем сколько единиц метров
        float desMet = (rx_bufferUART[7] - 0x30) * 0.1;   // По таблице ASCII отнимаем 48 и получаем сколько десятых долей метра
        float sotMet = (rx_bufferUART[8] - 0x30) * 0.01;  // По таблице ASCII отнимаем 48 и получаем сколько сотых долей метра
        float tysMet = (rx_bufferUART[9] - 0x30) * 0.001; // По таблице ASCII отнимаем 48 и получаем сколько тысячных долей метра
        distance = sot + des + met + desMet + sotMet + tysMet;
        // printf("Meas= %i - %i - %i . %.1f %.2f %.3f | ", sot, des, met, desMet, sotMet, tysMet);
        printf("Distance= %.3f \n", distance);
    }
    return distance;
}

void loop()
{
    // if (HAL_GetTick() - timeStart >= 10000)
    // {
    //     // laser80_stopMeasurement(0x80);
    // }
    if (dataUART1.flag == 1)
    {
        dataUART1.flag = 0;
        float dist = calcDistance(rx_bufferUART1, dataUART1.len);
        if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
        {
            printf("Dist UART%i = %.3f \n", dataUART1.num, dist);
            dataUART1.distance = dist;
        }
        else
        {
            printf("Error dataUART%i. \n", dataUART1.num);
        }
    }
    else if (dataUART2.flag == 1)
    {
        dataUART2.flag = 0;
    }
    else if (dataUART3.flag == 1)
    {
        dataUART3.flag = 0;
    }
    else if (dataUART4.flag == 1)
    {
        dataUART4.flag = 0;
    }

    if (flagContinius == 1)
    {
        flagContinius = 0;
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin); // Инвертирование состояния выхода.
    }

    // HAL_Delay(); // Пауза 500 миллисекунд.
    //----------------------------- 10 миллисекунд --------------------------------------
    if (flag_timer_10millisec)
    {
        flag_timer_10millisec = false;
        HAL_GPIO_TogglePin(Analiz1_GPIO_Port, Analiz1_Pin); // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(Led1_GPIO_Port, Led1_Pin);             // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
    }
    //----------------------------- 50 миллисекунд --------------------------------------
    if (flag_timer_50millisec)
    {
        flag_timer_50millisec = false;
        // HAL_GPIO_TogglePin(En_Motor_GPIO_Port, En_Motor_Pin);     // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(Dir_Motor0_GPIO_Port, Dir_Motor0_Pin); // Инвертирование состояния выхода.
        // flag_data = true; // Есть новые данные по шине // РУчной вариант имитации пришедших данных с частотой 20Гц
    }

    //----------------------------- 1 секунда --------------------------------------
    if (flag_timer_1sec) // Вызывается каждую секунду
    {
        HAL_GPIO_TogglePin(Led1_GPIO_Port, Led1_Pin); // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(Analiz2_GPIO_Port, Analiz2_Pin); // Инвертирование состояния выхода.
        //  uint8_t UART1_rxBuffer[4] = {0xAA,0xFF,0xAA,0xFF};
        //   uint8_t UART1_rxBuffer[1] = {0x56}; //Запрос версии "V"
        //   uint8_t UART1_rxBuffer[1] = {0x4F}; // Включить лазер "O"
        //   uint8_t UART1_rxBuffer[1] = {0x43}; // Выключить лазер "C"
        flag_timer_1sec = false;
    }
}

#endif /*CODE_H*/
