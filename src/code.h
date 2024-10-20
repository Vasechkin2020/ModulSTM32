#ifndef CODE_H
#define CODE_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

//********************************* ПЕРЕМЕННЫЕ ***************************************************************************

bool flag_timer_10millisec = false;
bool flag_timer_50millisec = false;
bool flag_timer_1sec = false;

GPIO_TypeDef *myPort;

void loop();
void timer6(); // Обработчик прерывания таймера TIM6	1 раз в 1 милисекунду

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
void SendDataDMA(uint8_t *data, uint16_t size)
{
    // Запускаем передачу данных по DMA
    if (HAL_UART_Transmit_DMA(&huart1, data, size) != HAL_OK)
    {
        // Обработка ошибки
        Error_Handler();
    }
}

// Когда DMA завершит передачу, произойдет прерывание, и вызовется функция обратного вызова HAL_UART_TxCpltCallback. В ней можно обработать завершение передачи, например, запустить передачу новых данных или освободить буфер.
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        // Обработка завершения передачи по UART1 // Например, можно вывести сообщение о том, что передача завершена
        HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin); // Инвертирование состояния выхода.
    }
}

void loop()
{

    // HAL_Delay(); // Пауза 500 миллисекунд.
    //----------------------------- 10 миллисекунд --------------------------------------
    if (flag_timer_10millisec)
    {
        flag_timer_10millisec = false;
        HAL_GPIO_TogglePin(Analiz1_GPIO_Port, Analiz1_Pin); // Инвертирование состояния выхода.
        // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
    }
    //----------------------------- 50 миллисекунд --------------------------------------
    if (flag_timer_50millisec)
    {
        flag_timer_50millisec = false;
        HAL_GPIO_TogglePin(Led1_GPIO_Port, Led1_Pin);             // Инвертирование состояния выхода.
        HAL_GPIO_TogglePin(En_Motor_GPIO_Port, En_Motor_Pin);     // Инвертирование состояния выхода.
        HAL_GPIO_TogglePin(Dir_Motor0_GPIO_Port, Dir_Motor0_Pin); // Инвертирование состояния выхода.
        // flag_data = true; // Есть новые данные по шине // РУчной вариант имитации пришедших данных с частотой 20Гц
    }

    //----------------------------- 1 секунда --------------------------------------
    if (flag_timer_1sec) // Вызывается каждую секунду
    {
        HAL_GPIO_TogglePin(Analiz2_GPIO_Port, Analiz2_Pin); // Инвертирование состояния выхода.
        // uint8_t UART1_rxBuffer[4] = {0xAA,0xFF,0xAA,0xFF};
        //  uint8_t UART1_rxBuffer[1] = {0x56}; //Запрос версии "V"
        //  uint8_t UART1_rxBuffer[1] = {0x4F}; // Включить лазер "O"
        //  uint8_t UART1_rxBuffer[1] = {0x43}; // Выключить лазер "C"
        //  uint8_t UART1_rxBuffer[5] = {0x80,0x06,0x05,0x01,0x74}; // Включить лазер 80M
         // uint8_t UART1_rxBuffer[5] = {0x80,0x06,0x05,0x00,0x75}; // Выключить лазер 80M
        // uint8_t UART1_rxBuffer[4] = {0xFA,0x06,0x04,0xFC}; // Включить лазер 80M
        //  uint8_t UART1_rxBuffer[4] = {0xFA,0x06,0x06,0xFA}; // Single measurement (broadcast)
       

        flag_timer_1sec = false;
    }
}

#endif /*CODE_H*/