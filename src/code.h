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
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size); // Коллбэк, вызываемый при событии UART Idle по окончания приема
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);                   // Коллбэк, вызываемый при событии UART по окончания приема ОПРЕДЕЛЕННОГО ЗАДАННОГО ЧИСЛА БАЙТ
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
    uint8_t status; // Номер UART
    float angle;    // Угол в котором находился мотор в момент когда пришли данные по измерению
    float distance; // Дистанция по последнему хорошему измерению
    uint8_t *adr;   // Адрес буфера
};

struct dataUART dataUART[4];
// struct dataUART dataUART2;
// struct dataUART dataUART3;
// struct dataUART dataUART4;

HAL_StatusTypeDef status;
int sss;

// void HAL_UART_RxIdleCallback(UART_HandleTypeDef *huart) НЕ ЗАРАБОТАЛО, НЕТ ВЫЗОВА ПО idle НИГДЕ КРОМЕ 1 UART НЕ смог разобраться почему
// {
//     if (huart->Instance == USART2)
//     {
//         dataUART2.flag = 1; // Обработка полученных данных
//         //dataUART2.len = Size;
//         dataUART2.num = 2;
//     }
// }

extern float getAngle(int32_t _pulse); // Пересчет импульсов в градусы

// Коллбэк, вызываемый при событии UART по окончания приема ОПРЕДЕЛЕННОГО ЗАДАННОГО ЧИСЛА БАЙТ
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        dataUART[1].flag = 1; // Обработка полученных данных
        dataUART[1].len = 11;
        dataUART[1].num = 1;
        dataUART[1].adr = rx_bufferUART2;
        dataUART[1].angle = getAngle(motor[1].position);
        status = HAL_UART_Receive_DMA(&huart2, rx_bufferUART2, 11); // После обработки вновь запустить прием
        dataUART[1].status = status;
    }
    else if (huart->Instance == USART3)
    {
        dataUART[2].flag = 1; // Обработка полученных данных
        dataUART[2].len = 11;
        dataUART[2].num = 2;
        dataUART[2].adr = rx_bufferUART3;
        dataUART[2].angle = getAngle(motor[2].position);
        status = HAL_UART_Receive_DMA(&huart3, rx_bufferUART3, 11); // После обработки вновь запустить прием
        dataUART[2].status = status;
    }
    else if (huart->Instance == USART4)
    {
        dataUART[3].flag = 1; // Обработка полученных данных
        dataUART[3].len = 11;
        dataUART[3].num = 3;
        dataUART[3].adr = rx_bufferUART4;
        dataUART[3].angle = getAngle(motor[3].position);
        status = HAL_UART_Receive_DMA(&huart4, rx_bufferUART4, 11); // После обработки вновь запустить прием
        dataUART[3].status = status;
    }
}

// Коллбэк, вызываемый при событии UART Idle по окончания приема
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1)
    {
        dataUART[0].flag = 1;
        dataUART[0].len = Size;
        dataUART[0].num = 0;
        dataUART[0].adr = rx_bufferUART1;
        dataUART[0].angle = getAngle(motor[0].position);
        status = HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // После обработки вновь запустить прием
        dataUART[0].status = status;
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
        // printf("Distance= %.3f \n", distance);
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
        Modul2Data_send.motor[i].status = motor[i].status;                                       // Считываем состояние пина драйверов
        Modul2Data_send.motor[i].position = getAngle(motor[i].position);                         // Записываем текущую позицию преобразуя из импульсов в градусы, надо еще в глобальную систему преобразовывать
        Modul2Data_send.motor[i].destination = getAngle(motor[i].destination);                   // Считываем цель по позиции, надо еще в глобальную систему преобразовывать
        Modul2Data_send.micric[i] = HAL_GPIO_ReadPin(motor[i].micric_port, motor[i].micric_pin); //
    }

    for (int i = 0; i < 4; i++) // Информация по лазерам по ситуации
    {

        if (Data2Modul_receive.controlLaser.mode == 1) // Если команда работать с датчиком
        {
            Modul2Data_send.laser[i].status = dataUART[i].status;                              // Считываем статаус дальномера
            Modul2Data_send.laser[i].distance = (float)dataUART[i].distance;                   // * 0.001;          // Считываем измерение растояния и пересчитываем в метры !!!
            Modul2Data_send.laser[i].signalQuality = 0;                                        // Считываем качество сигнала измерение
            Modul2Data_send.laser[i].angle = (float)dataUART[i].angle;                         // Считываем угол в котором произвели измерение
            Modul2Data_send.laser[i].numPillar = Data2Modul_receive.controlMotor.numPillar[i]; // Переписываем номер столба на который измеряли расстояние
        }
        else
        {
            Modul2Data_send.laser[i].status = 0;        // Считываем статаус дальномера
            Modul2Data_send.laser[i].distance = 0;      // Считываем измерение растояния и пересчитываем в метры !!!
            Modul2Data_send.laser[i].signalQuality = 0; // Считываем угол в котором произмели измерение
            Modul2Data_send.laser[i].angle = 0;         // Считываем угол в котором произвели измерение
            Modul2Data_send.laser[i].numPillar = -1;    // Переписываем номер столба на который измеряли расстояние
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

extern void setMotorAngle(int num, float _angle);
extern void setZeroMotor();

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
        // Непрерывное измерение
        laser80_continuousMeasurement(huart1, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart2, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart3, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_continuousMeasurement(huart4, 0x80); // Данные пойдут только через 500 милисекунд
    }
    // Команда ВЫЛЮЧЕНИЯ ЛАЗЕРНЫХ ДАТЧИКОВ
    if (Data2Modul_receive.controlLaser.mode == 0 && Data2Modul_receive.controlLaser.mode != laser_pred) // Если пришла команда и предыдущая была другая
    {
        // Непрерывное измерение
        laser80_stopMeasurement(huart1, 0x80);
        laser80_stopMeasurement(huart2, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_stopMeasurement(huart3, 0x80); // Данные пойдут только через 500 милисекунд
        laser80_stopMeasurement(huart4, 0x80); // Данные пойдут только через 500 милисекунд
    }

    mode_pred = Data2Modul_receive.controlMotor.mode;  // Запоминаяем команду
    laser_pred = Data2Modul_receive.controlLaser.mode; // Запоминаяем команду
                                                       //     // printf(" Data2Modul.radius= %f ", Data2Modul_receive.radius);
}

// Инициализация лазеров
void laserInit()
{

    // HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_bufferUART1, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx_bufferUART2, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx_bufferUART3, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
    // HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx_bufferUART4, RX_BUFFER_SIZE); // Двнные оказываются в буфере rx_bufferUART1
    // HAL_Delay(100);

    // laser80_setAddress(huart1, 0x80);
    // laser80_setAddress(huart2, 0x80);
    // laser80_setAddress(huart3, 0x80);
    // laser80_setAddress(huart4, 0x80);
    // HAL_Delay(1000);
    laser80_stopMeasurement(huart1, 0x80);
    laser80_stopMeasurement(huart2, 0x80);
    laser80_stopMeasurement(huart3, 0x80);
    laser80_stopMeasurement(huart4, 0x80);
    HAL_Delay(3000);
    laser80_controlLaser(huart1, 1, 0x80);
    laser80_controlLaser(huart2, 1, 0x80);
    laser80_controlLaser(huart3, 1, 0x80);
    laser80_controlLaser(huart4, 1, 0x80);
    HAL_Delay(1000);
    laser80_setTimeInterval(huart1, 0);
    laser80_setTimeInterval(huart2, 0);
    laser80_setTimeInterval(huart3, 0);
    laser80_setTimeInterval(huart4, 0);
    HAL_Delay(1000);
    laser80_setResolution(huart1, 1);
    laser80_setResolution(huart2, 1);
    laser80_setResolution(huart3, 1);
    laser80_setResolution(huart4, 1);
    HAL_Delay(1000);
    laser80_setRange(huart1, 30);
    laser80_setRange(huart2, 30);
    laser80_setRange(huart3, 30);
    laser80_setRange(huart4, 30);
    HAL_Delay(1000);
    laser80_setStartingPoint(huart1, 1);
    laser80_setStartingPoint(huart2, 1);
    laser80_setStartingPoint(huart3, 1);
    laser80_setStartingPoint(huart4, 1);
    HAL_Delay(1000);
    laser80_setFrequency(huart1, 10);
    laser80_setFrequency(huart2, 10);
    laser80_setFrequency(huart3, 10);
    laser80_setFrequency(huart4, 10);
    HAL_Delay(1000);

    // Непрерывное измерение
    laser80_continuousMeasurement(huart1, 0x80); // Данные пойдут только через 500 милисекунд
    laser80_continuousMeasurement(huart2, 0x80); // Данные пойдут только через 500 милисекунд
    laser80_continuousMeasurement(huart3, 0x80); // Данные пойдут только через 500 милисекунд
    laser80_continuousMeasurement(huart4, 0x80); // Данные пойдут только через 500 милисекунд

    // HAL_Delay(5000);
    // laser80_stopMeasurement(huart1,0x80);
}

void loop()
{
    // if (HAL_GetTick() - timeStart >= 10000)
    // {
    //     // laser80_stopMeasurement(0x80);
    // }
    if (millis() - timeSpi > 3000) // Если обмена нет больше 5 секунд то отключаем все
    {
        Data2Modul_receive.controlLaser.mode = 0;                          // Отключаем лазерные датчики
        Data2Modul_receive.controlMotor.mode = 0;                          // Отключаем моторы
        laser80_stopMeasurement(huart1,0x80);
        laser80_stopMeasurement(huart2,0x80);
        laser80_stopMeasurement(huart3,0x80);
        laser80_stopMeasurement(huart4,0x80);
        HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
    }
    //----------------------------- По факту обмена данными с верхним уровнем --------------------------------------
#ifdef SPI_protocol
    if (flag_data) // Если обменялись данными
    {

        flag_data = false;
        // workingSPI();
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

    for (int i = 0; i < 4; i++)
    {
        if (dataUART[i].flag == 1)
        {
            dataUART[i].flag = 0;
            // printf("%li Len%i = %i status= %i   /   ", millis(), dataUART[i].num,  dataUART[i].len, dataUART[i].status);
            for (int i = 0; i < dataUART[i].len; i++)
            {
                // printf("%x ", rx_bufferUART1[i]);
            }
            // printf("\r\n");
            float dist = calcDistance(dataUART[i].adr, dataUART[i].len);
            if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
            {
                // printf(" UART%i = %.3f \r\n", dataUART[i].num, dist);
                dataUART[i].distance = dist;
            }
            else
            {
                printf("Error dataUART%i. \r\n", dataUART[i].num);
            }
            memset(dataUART[i].adr, 0, RX_BUFFER_SIZE); // Очистка буфера
        }
    }

    // if (dataUART[0].flag == 1)
    // {
    //     dataUART[0].flag = 0;
    //     printf("%li Len1 = %i status= %i   /   ", millis(), dataUART[0].len, dataUART[0].status);
    //     for (int i = 0; i < dataUART[0].len; i++)
    //     {
    //         // printf("%x ", rx_bufferUART1[i]);
    //     }
    //     // printf("\r\n");
    //     float dist = calcDistance(rx_bufferUART1, dataUART[0].len);
    //     if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
    //     {
    //         printf(" UART%i = %.3f \r\n", dataUART[0].num, dist);
    //         dataUART[0].distance = dist;
    //     }
    //     else
    //     {
    //         printf("Error dataUART%i. \r\n", dataUART[0].num);
    //     }
    //     memset(dataUART[0].adr, 0, RX_BUFFER_SIZE); // Очистка буфера
    // }
    // else if (dataUART[1].flag == 1)
    // {
    //     dataUART[1].flag = 0;

    //     printf("%li Len2 = %i status= %i   /   ", millis(), dataUART[1].len, dataUART[1].status);
    //     sss = 0;
    //     for (int i = 0; i < dataUART[1].len; i++)
    //     {
    //         // printf("%x ", rx_bufferUART2[i]);
    //     }
    //     // printf("\r\n");

    //     float dist = calcDistance(rx_bufferUART2, dataUART[1].len);
    //     if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
    //     {
    //         printf(" UART%i = %.3f \r\n", dataUART[1].num, dist);
    //         dataUART[1].distance = dist;
    //     }
    //     else
    //     {
    //         printf("Error dataUART%i. \n", dataUART[1].num);
    //     }
    //     memset(rx_bufferUART2, 0, RX_BUFFER_SIZE); // Очистка буфера
    // }
    // else if (dataUART[2].flag == 1)
    // {
    //     dataUART[2].flag = 0;
    //     printf("%li Len3 = %i status= %i   /   ", millis(), dataUART[2].len, dataUART[2].status);
    //     for (int i = 0; i < dataUART[2].len; i++)
    //     {
    //         // printf("%x ", rx_bufferUART3[i]);
    //     }
    //     // printf("\r\n");

    //     float dist = calcDistance(rx_bufferUART3, dataUART[2].len);
    //     if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
    //     {
    //         printf(" UART%i = %.3f \r\n", dataUART[2].num, dist);
    //         dataUART[2].distance = dist;
    //     }
    //     else
    //     {
    //         printf("Error dataUART%i. \n", dataUART[2].num);
    //     }
    //     memset(rx_bufferUART3, 0, RX_BUFFER_SIZE); // Очистка буфера
    // }
    // else if (dataUART[3].flag == 1)
    // {
    //     dataUART[3].flag = 0;
    //     // printf("%li Len4 = %i ", millis(), dataUART[3].len);
    //     // float dist = calcDistance(rx_bufferUART4, dataUART[3].len);
    //     // if (dist != 0) // Расчитываем дистанцию. Возвращаем значение или 0 если ошибка
    //     // {
    //     //     printf("Dist UART%i = %.3f \n", dataUART[3].num, dist);
    //     //     dataUART[3].distance = dist;
    //     // }
    //     // else
    //     // {
    //     //     printf("Error dataUART%i. \n", dataUART[1].num);
    //     // }
    //     // memset(rx_bufferUART4, 0, RX_BUFFER_SIZE); // Очистка буфера
    // }

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

#endif /*CODE_H*/
