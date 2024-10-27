#ifndef CODE_H
#define CODE_H

#define MOTOR yes
#define SPI_protocol yes
#define LASER yes

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "laser80M.h"
#include "slaveSPI.h"

//********************************* ПЕРЕМЕННЫЕ ***************************************************************************

bool flag_timer_10millisec = false;
bool flag_timer_50millisec = false;
bool flag_timer_1sec = false;

GPIO_TypeDef *myPort;

void loop();
void timer6(); // Обработчик прерывания таймера TIM6	1 раз в 1 милисекунду
float distanceUART1 = 0;
int flagContinius = 0;

extern volatile uint32_t millisCounter;

//********************************* ФУНКЦИИ ***************************************************************************
// Функция для возврата количества миллисекунд
uint32_t millis(void)
{
    return millisCounter;
}

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

// Собираем нужные данные и пишем в структуру на отправку
void collect_Data_for_Send()
{
    Modul2Data_send.id++;
    // Modul2Data_send.pinMotorEn = digitalRead(PIN_Motor_En); // Считываем состояние пина драйверов

    for (int i = 0; i < 4; i++) // Информация по моторам всегда
    {
        // Modul2Data_send.motor[i].status = motor[i].status; // Считываем состояние пина драйверов
        // Modul2Data_send.motor[i].position = tfLocalToGlobal360(getAngle(motor[i].position), i);       // Записываем текущую позицию преобразуя из импульсов в градусы, надо еще в глобальную систему преобразовывать
        // Modul2Data_send.motor[i].destination = tfLocalToGlobal360(getAngle(motor[i].destination), i); // Считываем цель по позиции, надо еще в глобальную систему преобразовывать
        // Modul2Data_send.motor[i].position = getAngle(motor[i].position);       // Записываем текущую позицию преобразуя из импульсов в градусы, надо еще в глобальную систему преобразовывать
        // Modul2Data_send.motor[i].destination = getAngle(motor[i].destination); // Считываем цель по позиции, надо еще в глобальную систему преобразовывать
        // Modul2Data_send.micric[i] = digitalRead(motor[i].micric_pin);          //
    }

    for (int i = 0; i < 4; i++) // Информация по лазерам по ситуации
    {

        if (Data2Modul_receive.controlLaser.mode == 1) // Если команда работать с датчиком
        {
            // Modul2Data_send.laser[i].status = sk60plus[i]._status;                             // Считываем статаус дальномера
            // Modul2Data_send.laser[i].distance = (float)sk60plus[i]._distance * 0.001;          // Считываем измерение растояния и пересчитываем в метры !!!
            // Modul2Data_send.laser[i].signalQuality = sk60plus[i]._signalQuality;               // Считываем угол в котором произмели измерение
            // Modul2Data_send.laser[i].angle = sk60plus[i]._angle;                               // Считываем угол в котором произвели измерение
            // Modul2Data_send.laser[i].numPillar = Data2Modul_receive.controlMotor.numPillar[i]; // Переписываем номер столба на который измеряли расстояние
        }
        else
        {
            // Modul2Data_send.laser[i].status = 0;        // Считываем статаус дальномера
            // Modul2Data_send.laser[i].distance = 0;      // Считываем измерение растояния и пересчитываем в метры !!!
            // Modul2Data_send.laser[i].signalQuality = 0; // Считываем угол в котором произмели измерение
            // Modul2Data_send.laser[i].angle = 0;         // Считываем угол в котором произвели измерение
            // Modul2Data_send.laser[i].numPillar = -1;    // Переписываем номер столба на который измеряли расстояние
        }
    }

    Modul2Data_send.spi.all = spi.all;
    Modul2Data_send.spi.bed = spi.bed;
    uint32_t cheksum_send = 0;                                          // Считаем контрольную сумму отправляемой структуры
    unsigned char *adr_structura = (unsigned char *)(&Modul2Data_send); // Запоминаем адрес начала структуры. Используем для побайтной передачи
    for (int i = 0; i < sizeof(Modul2Data_send) - 4; i++)
    {
        cheksum_send += adr_structura[i]; // Побайтно складываем все байты структуры кроме последних 4 в которых переменная в которую запишем результат
    }
    Modul2Data_send.cheksum = cheksum_send;
    // Modul2Data_send.cheksum = measureCheksum_Modul2Data(Modul2Data_send); // Вычисляем контрольную сумму структуры и пишем ее значение в последний элемент
}

// Отработка пришедших команд. Изменение скорости, траектории и прочее
void executeDataReceive()
{
  static int mode_pred = 0; // Переменная для запоминания предыдущей команды
  // Команда УПРАВЛЕНИЯ УГЛАМИ
  if (Data2Modul_receive.controlMotor.mode == 0) // Если пришла команда 0 Управления
  {
    // Ничего не делаем
  }
  if (Data2Modul_receive.controlMotor.mode == 1) // Если пришла команда 1 Управления
  {
    for (int i = 0; i < 4; i++)
    {
      //setMotorAngle(i, Data2Modul_receive.controlMotor.angle[i]);
    }
  }
  // Команда КОЛИБРОВКИ И УСТАНОВКИ В 0
  if (Data2Modul_receive.controlMotor.mode == 9 && Data2Modul_receive.controlMotor.mode != mode_pred) // Если пришла команда 9 Колибровки и предыдущая была другая
  {
    //setZeroMotor(); // Установка в ноль
  }

  mode_pred = Data2Modul_receive.controlMotor.mode; // Запоминаяем команду
  //     // printf(" Data2Modul.radius= %f ", Data2Modul_receive.radius);
}


void loop()
{
    // if (HAL_GetTick() - timeStart >= 10000)
    // {
    //     // laser80_stopMeasurement(0x80);
    // }

    //----------------------------- По факту обмена данными с верхним уровнем --------------------------------------
#ifdef SPI_protocol
    if (flag_data) // Если обменялись данными
    {
        flag_data = false;
        timeSpi = millis(); // Запоминаем время обмена
        // printf("+\n");
        processingDataReceive(); // Обработка пришедших данных после состоявшегося обмена  !!! Подумать почему меняю данные даже если они с ошибкой, потом по факту когда будет все работать
        // printf(" mode= %i \n",Data2Modul_receive.controlMotor.mode);
        executeDataReceive(); // Выполнение пришедших команд

        // printf(" Receive id= %i cheksum= %i command= %i ", Data2Modul_receive.id, Data2Modul_receive.cheksum,Data2Modul_receive.command );
        // printf(" All= %i bed= %i ", spi.all, spi.bed);
        // printf(" angle0= %.2f angle1= %.2f angle2= %.2f angle3= %.2f", Data2Modul_receive.angle[0], Data2Modul_receive.angle[1], Data2Modul_receive.angle[2], Data2Modul_receive.angle[3] );

        collect_Data_for_Send(); // Собираем данные в структуре для отправки на момент прихода команлы, но БЕЗ учета команды.До исполнения команды.
        spi_slave_queue_Send(); // Закладываем данные в буфер для передачи(обмена)
    }
#endif

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
