#ifndef CODE_H
#define CODE_H

#define MOTOR yes
#define SPI_protocol yes
#define LASER yes

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "laser80M.h"
#include "sk60plus.h"
#include "slaveSPI.h"

//********************************* ПЕРЕМЕННЫЕ ***************************************************************************

bool flag_timer_10millisec = false;
bool flag_timer_50millisec = false;
bool flag_timer_1sec = false;

GPIO_TypeDef *myPort;

void loop();
void timer6();                                                             // Обработчик прерывания таймера TIM6	1 раз в 1 милисекунду
void workingTimer();                                                       // Отработка действий по таймеру в 1, 50, 60 милисекунд
void workingLaser();                                                       // Отработка действий по лазерным датчикам
void workingSPI();                                                         // Отработка действий по обмену по шине SPI
void workingStopTimeOut();                                                 // Остановка дазеоров и моторов при обрыве связи
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size); // Коллбэк, вызываемый при событии UART Idle по окончания приема
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);                   // Коллбэк, вызываемый при событии UART по окончания приема ОПРЕДЕЛЕННОГО ЗАДАННОГО ЧИСЛА БАЙТ
void laserInit();                                                          // Инициализация лазеров зависимоти от типа датчкика. определяем переменные буфер приема для каждого UART

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
    uint8_t *adr;      // Адрес буфера
    UART_HandleTypeDef *huart;
};

struct dataUART dataUART[4];
uint8_t lenDataLaser; // Длинна полученных данных в буфере
HAL_StatusTypeDef status;

bool flagTimeOut = true;       // Флаг таймаута при обрыве связи по SPI
bool flagCallBackUart = false; // Флаг для указания нужно ли отрабатывать в колбеке  или обраьотка с самой функции

extern float getAngle(int32_t _pulse); // Пересчет импульсов в градусы
extern void setMotorAngle(int num, float _angle);
extern void setZeroMotor();
extern volatile uint32_t millisCounter;

// typedef struct SDataLaser
// {
//     uint32_t distance;
//     uint16_t signalQuality;
// } SDataLaser;

// SDataLaser dataLaser[4]; // Структура куда пишем даные из датчиков

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

    millisCounter++; // Увеличиваем счетчик миллисекунд

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

void workingTimer() // Отработка действий по таймеру в 1, 50, 60 милисекунд
{
    // HAL_Delay(); // Пауза 500 миллисекунд.
    //----------------------------- 10 миллисекунд --------------------------------------
    if (flag_timer_10millisec)
    {
        flag_timer_10millisec = false;
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
        printf("%li \r\n", millis());
        // HAL_GPIO_TogglePin(Analiz2_GPIO_Port, Analiz2_Pin); // Инвертирование состояния выхода.
        //  uint8_t UART1_rxBuffer[4] = {0xAA,0xFF,0xAA,0xFF};
        //   uint8_t UART1_rxBuffer[1] = {0x56}; //Запрос версии "V"
        //   uint8_t UART1_rxBuffer[1] = {0x4F}; // Включить лазер "O"
        //   uint8_t UART1_rxBuffer[1] = {0x43}; // Выключить лазер "C"
        flag_timer_1sec = false;
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

// void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart) НЕ ЗАРАБОТАЛО, НЕТ ВЫЗОВА ПО idle НИГДЕ КРОМЕ 1 UART НЕ смог разобраться почему
// {
//     if (huart->Instance == USART2)
//     {
//         dataUART2.flag = 1; // Обработка полученных данных
//         //dataUART2.len = Size;
//         dataUART2.num = 2;
//     }
// }

// Коллбэк, вызываемый при событии UART по окончания приема ОПРЕДЕЛЕННОГО ЗАДАННОГО ЧИСЛА БАЙТ
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (flagCallBackUart) // Флаг указывает нужно ли отрабатывать колбек или мы в самой функции вызова ждем результат и обрабатываем
    {

        if (huart->Instance == USART1)
        {
            dataUART[0].flag = 1;                                                 // Обработка полученных данных
            status = HAL_UART_Receive_DMA(&huart1, rx_bufferUART1, lenDataLaser); // После обработки вновь запустить прием
            dataUART[0].statusDMA = status;
        }
        else if (huart->Instance == USART2)
        {
            dataUART[1].flag = 1;                                                 // Обработка полученных данных
            status = HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, lenDataLaser); // После обработки вновь запустить прием
            dataUART[1].statusDMA = status;
        }
        else if (huart->Instance == USART3)
        {
            dataUART[2].flag = 1;                                                 // Обработка полученных данных
            status = HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, lenDataLaser); // После обработки вновь запустить прием
            dataUART[2].statusDMA = status;
        }
        else if (huart->Instance == USART4)
        {
            dataUART[3].flag = 1;                                                 // Обработка полученных данных
            status = HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, lenDataLaser); // После обработки вновь запустить прием
            dataUART[3].statusDMA = status;
        }
    }
}

// Коллбэк, вызываемый при событии UART Idle по окончания приема
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
// {
//     if (huart->Instance == USART1)
//     {
//         dataUART[0].flag = 1;
//         dataUART[0].len = Size;
//         dataUART[0].num = 0;
//         dataUART[0].adr = rx_bufferUART1;
//         dataUART[0].angle = getAngle(motor[0].position);
//         status = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // После обработки вновь запустить прием
//         dataUART[0].status = status;
//     }
// }

// Собираем нужные данные и пишем в структуру на отправку
void collect_Data_for_Send()
{
    Modul2Data_send.id++;
    // Modul2Data_send.pinMotorEn = digitalRead(PIN_Motor_En); // Считываем состояние пина драйверов

    for (int i = 0; i < 4; i++) // Информация по моторам всегда
    {
        Modul2Data_send.motor[i].status = motor[i].status;                                       // Считываем состояние пина драйверов
        Modul2Data_send.motor[i].position = getAngle(motor[i].position);                         // Записываем текущую позицию преобразуя из импульсов в градусы, надо еще в глобальную систему преобразовывать
        Modul2Data_send.motor[i].destination = getAngle(motor[i].destination);                   // Считываем цель по позиции, надо еще в глобальную систему преобразовывать
        Modul2Data_send.micric[i] = HAL_GPIO_ReadPin(motor[i].micric_port, motor[i].micric_pin); //
    }

    for (int i = 0; i < 4; i++) // Информация по лазерам по ситуации
    {

        if (Data2Modul_receive.controlLaser.mode != 0) // Если команда работать с датчиком
        {
            Modul2Data_send.laser[i].status = dataUART[i].status;                              // Считываем статаус дальномера
            Modul2Data_send.laser[i].distance = (float)dataUART[i].distance;                   // * 0.001;          // Считываем измерение растояния и пересчитываем в метры !!!
            Modul2Data_send.laser[i].signalQuality = dataUART[i].quality;                      // Считываем качество сигнала измерение
            Modul2Data_send.laser[i].angle = (float)dataUART[i].angle;                         // Считываем угол в котором произвели измерение
            Modul2Data_send.laser[i].time = dataUART[i].time;                                  // Считываем время в котором произвели измерение
            Modul2Data_send.laser[i].numPillar = Data2Modul_receive.controlMotor.numPillar[i]; // Переписываем номер столба на который измеряли расстояние
        }
        else
        {
            Modul2Data_send.laser[i].status = 888; // Статус не работаем с датчиком
            Modul2Data_send.laser[i].distance = 0;
            Modul2Data_send.laser[i].signalQuality = 0;
            Modul2Data_send.laser[i].angle = 0;
            Modul2Data_send.laser[i].time = 0;
            Modul2Data_send.laser[i].numPillar = -1; // Номер не существующего столба
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
    static int mode_pred = 0;  // Переменная для запоминания предыдущей команды
    static int laser_pred = 0; // Переменная для запоминания предыдущей команды
    // Команда УПРАВЛЕНИЯ УГЛАМИ
    if (Data2Modul_receive.controlMotor.mode == 0) // Если пришла команда 0 Управления
    {
        // Ничего не делаем
    }
    if (Data2Modul_receive.controlMotor.mode == 1) // Если пришла команда 1 Управления
    {
        for (int i = 0; i < 4; i++)
        {
            // printf("executeDataReceive = %i status = %i \r\n",Data2Modul_receive.controlMotor.mode,motor[i].status);
            setMotorAngle(i, Data2Modul_receive.controlMotor.angle[i]);
            // printf("status = %i \r\n", motor[i].status);
        }
    }
    // Команда КОЛИБРОВКИ И УСТАНОВКИ В 0
    if (Data2Modul_receive.controlMotor.mode == 9 && Data2Modul_receive.controlMotor.mode != mode_pred) // Если пришла команда 9 Колибровки и предыдущая была другая
    {
        setZeroMotor(); // Установка в ноль
    }
    // Команда ВКЛЮЧЕНИЯ ЛАЗЕРНЫХ ДАТЧИКОВ
    if (Data2Modul_receive.controlLaser.mode == 1 && Data2Modul_receive.controlLaser.mode != laser_pred) // Если пришла команда и предыдущая была другая
    {
#ifdef LASER80
        // Непрерывное измерение
        laser80_continuousMeasurement(huart1, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart2, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart3, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart4, 0x80); // Данные пойдут только через 500 милисекунд
#endif
#ifdef LASER60
        sk60plus_startContinuousSlow(&huart1, rx_bufferUART1);
        sk60plus_startContinuousSlow(&huart2, rx_bufferUART2);
        sk60plus_startContinuousSlow(&huart3, rx_bufferUART3);
        sk60plus_startContinuousSlow(&huart4, rx_bufferUART4);
#endif
    }
    if (Data2Modul_receive.controlLaser.mode == 2 && Data2Modul_receive.controlLaser.mode != laser_pred) // Если пришла команда и предыдущая была другая
    {
#ifdef LASER60
        sk60plus_startContinuousAuto(&huart1, rx_bufferUART1);
        sk60plus_startContinuousAuto(&huart2, rx_bufferUART2);
        sk60plus_startContinuousAuto(&huart3, rx_bufferUART3);
        sk60plus_startContinuousAuto(&huart4, rx_bufferUART4);
#endif
    }
    // Команда ВЫЛЮЧЕНИЯ ЛАЗЕРНЫХ ДАТЧИКОВ
    if (Data2Modul_receive.controlLaser.mode == 0 && Data2Modul_receive.controlLaser.mode != laser_pred) // Если пришла команда и предыдущая была другая
    {
#ifdef LASER80
        laser80_stopMeasurement(huart1, 0x80);
        laser80_stopMeasurement(huart2, 0x80);
        laser80_stopMeasurement(huart3, 0x80);
        laser80_stopMeasurement(huart4, 0x80);
#endif
#ifdef LASER60
        sk60plus_stopContinuous(&huart1);
        sk60plus_stopContinuous(&huart2);
        sk60plus_stopContinuous(&huart3);
        sk60plus_stopContinuous(&huart4);
#endif
    }

    mode_pred = Data2Modul_receive.controlMotor.mode;  // Запоминаяем команду
    laser_pred = Data2Modul_receive.controlLaser.mode; // Запоминаяем команду
                                                       //     // printf(" Data2Modul.radius= %f ", Data2Modul_receive.radius);
}

void laserInit() // Инициализация лазеров в зависимоти от типа датчика. определяем переменные и буфер приема для каждого UART
{
    // Это общие данные для любых датчиков
    dataUART[0].num = 0;
    dataUART[0].adr = rx_bufferUART1;
    dataUART[0].huart = &huart1;

    dataUART[1].num = 1;
    dataUART[1].adr = rx_bufferUART2;
    dataUART[1].huart = &huart2;

    dataUART[2].num = 2;
    dataUART[2].adr = rx_bufferUART3;
    dataUART[2].huart = &huart3;

    dataUART[3].num = 3;
    dataUART[3].adr = rx_bufferUART4;
    dataUART[3].huart = &huart4;

#ifdef LASER80
    lenDataLaser = 11;

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
    HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, 11);                     // Двнные оказываются в буфере rx_bufferUART1
    HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, 11);                     // Двнные оказываются в буфере rx_bufferUART1
    HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, 11);                     // Двнные оказываются в буфере rx_bufferUART1

    laser80_Init(); // Инициализация лазеров

    sk60plus_autoBaund(huart1);
    sk60plus_autoBaund(huart2);
    sk60plus_autoBaund(huart3);
    sk60plus_autoBaund(huart4);

    // Это делаю что-бы нормально работало, а то похоже буфер сбивается и фигня выходит
    HAL_UART_DMAStop(&huart1); // Остановка DMA
    HAL_UART_DMAStop(&huart2); // Остановка DMA
    HAL_UART_DMAStop(&huart3); // Остановка DMA
    HAL_UART_DMAStop(&huart4); // Остановка DMA

    memset(rx_bufferUART1, 0, RX_BUFFER_SIZE); // Очистка буфера
    memset(rx_bufferUART2, 0, RX_BUFFER_SIZE); // Очистка буфера
    memset(rx_bufferUART3, 0, RX_BUFFER_SIZE); // Очистка буфера
    memset(rx_bufferUART4, 0, RX_BUFFER_SIZE); // Очистка буфера

    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
    HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, 11);                     // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
    HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, 11);                     // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA
    HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, 11);                     // Данные оказываются в буфере rx_bufferUART1//  // Перезапуск приема данных через DMA

#endif

#ifdef LASER60

    lenDataLaser = 13;
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // БЕЗ ЭТОЙ СТРОКИ НЕ РАБОТАЕТ ХРЕН ЗНАЕТ ПОЧЕМУ
    HAL_UART_DMAStop(&huart1);                                             // Остановка DMA
    HAL_UART_DMAStop(&huart2);                                             // Остановка DMA
    HAL_UART_DMAStop(&huart3);                                             // Остановка DMA
    HAL_UART_DMAStop(&huart4);                                             // Остановка DMA
    HAL_UART_Receive_DMA(&huart1, rx_bufferUART1, lenDataLaser);           // Данные оказываются в буфере rx_bufferUART1
    HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, lenDataLaser);           // Данные оказываются в буфере rx_bufferUART2
    HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, lenDataLaser);           // Данные оказываются в буфере rx_bufferUART3
    HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, lenDataLaser);           // Данные оказываются в буфере rx_bufferUART4

    sk60plus_autoBaund();

    sk60plus_setLaser(&huart1, 1);
    sk60plus_setLaser(&huart2, 1);
    sk60plus_setLaser(&huart3, 1);

    sk60plus_readSerialNumber(&huart1);
    sk60plus_readSerialNumber(&huart2);
    sk60plus_readSerialNumber(&huart3);

    sk60plus_readSoftwareVersion(&huart1);
    sk60plus_readSoftwareVersion(&huart2);
    sk60plus_readSoftwareVersion(&huart3);

    sk60plus_readHardwareVersion(&huart1);
    sk60plus_readHardwareVersion(&huart2);
    sk60plus_readHardwareVersion(&huart3);

    sk60plus_readInputVoltage(&huart1);
    sk60plus_readInputVoltage(&huart2);
    sk60plus_readInputVoltage(&huart3);

    sk60plus_setLaser(&huart1, 0);
    sk60plus_setLaser(&huart2, 0);
    sk60plus_setLaser(&huart3, 0);

    sk60plus_startSingleAuto(&huart1);
    sk60plus_startSingleAuto(&huart2);
    sk60plus_startSingleAuto(&huart3);

    // sk60plus_startContinuousAuto(&huart1, rx_bufferUART1);

#endif
}
// Отработка действий по лазерным датчикам
void workingLaser()
{
    for (int i = 0; i < 4; i++)
    {
        if (dataUART[i].flag == 1)
        {
            dataUART[i].flag = 0;

#ifdef LASER80
            dataUART[i].distance = laser80_calcDistance(dataUART[i].adr, lenDataLaser);
#endif

#ifdef LASER60
            if (dataUART[i].adr[0] == 0xAA) // Если ответ без ошибки то
            {
                dataUART[i].status = 0; // Статус все хорошо
                dataUART[i].distance = laser60_calcDistance(dataUART[i].adr);
                dataUART[i].quality = laser60_calcSignalQuality(dataUART[i].adr);
                dataUART[i].angle = getAngle(motor[i].position);
                dataUART[i].time = millisCounter;
                printf(" UART%i dist = %lu qual = %u \r\n", dataUART[i].num, dataUART[i].distance, dataUART[i].quality);
            }
            else
            {
                dataUART[i].status = 999; // Статус ошибка
                dataUART[i].distance = 0;
                dataUART[i].quality = 0;
                dataUART[i].angle = 0;
                dataUART[i].time = 0;
                printf("%li UART%i statusDMA= %i   /   ", millis(), dataUART[i].num, dataUART[i].statusDMA);
                for (int j = 0; j < lenDataLaser; j++)
                {
                    printf("%x ", dataUART[i].adr[j]);
                }
                printf(" Error dataUART%i. \r\n", dataUART[i].num);
                do
                {
                    HAL_UART_DMAStop(dataUART[i].huart);
                    memset(dataUART[i].adr, 0, RX_BUFFER_SIZE);                                      // Очистка буфера
                    status = HAL_UART_Receive_DMA(dataUART[i].huart, dataUART[i].adr, lenDataLaser); // Запускаем ожидание ответа, указываем куда и сколько байт мы ждем.
                    // printf("New status0 = %i ", status);
                    HAL_Delay(1);

                } while (status != 0);
                printf("New statusDMA = %i\r\n", status);
            }
#endif
        }
    }
}
// Отработка действий по обмену по шине SPI
void workingSPI()
{
    //----------------------------- По факту обмена данными с верхним уровнем --------------------------------------
#ifdef SPI_protocol
    if (flag_data) // Если обменялись данными
    {
        flag_data = false;
        flagTimeOut = true; // Флаг для выключения по таймауту
        timeSpi = millis(); // Запоминаем время обмена
        // printf ("In = %#x %#x %#x %#x \r\n",rxBuffer[0],rxBuffer[1],rxBuffer[2],rxBuffer[3]);
        // printf ("Out = %#x %#x %#x %#x \r\n",txBuffer[0],txBuffer[1],txBuffer[2],txBuffer[3]);
        // printf("+\n");
        processingDataReceive(); // Обработка пришедших данных после состоявшегося обмена  !!! Подумать почему меняю данные даже если они с ошибкой, потом по факту когда будет все работать
        // printf(" mode= %i \n",Data2Modul_receive.controlMotor.mode);
        executeDataReceive(); // Выполнение пришедших команд

        // printf(" Receive id= %i cheksum= %i command= %i ", Data2Modul_receive.id, Data2Modul_receive.cheksum,Data2Modul_receive.command );
        // printf(" All= %i bed= %i ", spi.all, spi.bed);
        // printf(" angle0= %.2f angle1= %.2f angle2= %.2f angle3= %.2f", Data2Modul_receive.angle[0], Data2Modul_receive.angle[1], Data2Modul_receive.angle[2], Data2Modul_receive.angle[3] );

        collect_Data_for_Send(); // Собираем данные в структуре для отправки на момент прихода команлы, но БЕЗ учета команды.До исполнения команды.
        spi_slave_queue_Send();  // Закладываем данные в буфер для передачи(обмена)
        // HAL_SPI_TransmitReceive_DMA(&hspi1, txBuffer, rxBuffer, BUFFER_SIZE); // Запуск обмена данными по SPI с использованием DMA
    }
#endif
}
// Остановка лазеров и моторов при обрыве связи
void workingStopTimeOut()
{
    // if (HAL_GetTick() - timeStart >= 10000)
    // {
    //     // laser80_stopMeasurement(0x80);
    // }
    if (flagTimeOut) // Если бы обмен
    {
        if (millis() - timeSpi > 3000) // Если обмена нет больше 5 секунд то отключаем все
        {
            flagTimeOut = false;
            HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_SET); // Отключаем моторы// Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
#ifdef LASER80
            laser80_stopMeasurement(huart1, 0x80);
            laser80_stopMeasurement(huart2, 0x80);
            laser80_stopMeasurement(huart3, 0x80);
            laser80_stopMeasurement(huart4, 0x80);
#endif
#ifdef LASER60
            sk60plus_stopContinuous(&huart1);
            sk60plus_stopContinuous(&huart2);
            sk60plus_stopContinuous(&huart3);
            sk60plus_stopContinuous(&huart4);
#endif
            // HAL_GPIO_WritePin(laserEn_GPIO_Port, laserEn_Pin, GPIO_PIN_RESET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
            // HAL_GPIO_WritePin(laserEn_GPIO_Port, laserEn_Pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
        }
    }
}

void loop()
{
    workingSPI();         // Отработка действий по обмену по шине SPI
    workingLaser();       // Отработка действий по лазерным датчикам
    workingTimer();       // Отработка действий по таймеру в 1, 50, 60 милисекунд
    workingStopTimeOut(); // Остановка драйверов и моторов при обрыве связи
}

#endif /*CODE_H*/
