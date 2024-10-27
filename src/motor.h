#ifndef MOTOR_H
#define MOTOR_H

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

//---------------------------------------------------------------------------------------
#define SPEED 1.0     // Скорость на всех моторах одинаковая максимально возможная, что-бы перемещаться как можно быстрее оборотов за секунду rps
#define MICROSTEP 16  // Микрошаг на драйверах
#define REDUKTOR 1    // Параметры Редуктора 1 - Нет редуктора
#define STEPMOTOR 0.9 // Параметры градусы на шаг

int timeingStep;                                       // Число тактов для таймера через которое нужно дать новый имульс мотору
int statusTestMotor = 0;                               // Статус теста мотора для отладки
const float sector = STEPMOTOR / REDUKTOR / MICROSTEP; // Столько градусов приходится на 1 импульс

void setSpeedMotor(float _speed);            // Функция устанавлявающая скорость вращения на ВСЕХ моторах, задается в оборотах за секунду rps
void initMotor();                            // Функция инциализации моторов
void disableMotor();                         // Отключение моторов в простое
void timer7();                               // Обработчик прерывания таймера TIM7
void Set_Timer7_Period(uint32_t new_period); // Функция для изменения периода таймера TIM7
void testMotorRun();                         // Запуск моторов на тест
void testMotorStop();                        // Остановка моторов после теста
void disableMotor();                         // Отключение моторов в простое
int32_t getPulse(float _angle);              // Пересчет градусов в импульсы
float getAngle(int32_t _pulse);              // Пересчет импульсов в градусы
void setMotorAngle(int num, float _angle);   // Установка мотора в нужное положение в градусах локальной системы
void setZeroMotor();                         // Функция установки в ноль всех моторов

void isrMicMotor0(); // Обработка прерывания по микрику на пине для мотора0
void isrMicMotor1(); // Обработка прерывания по микрику на пине для мотора1
void isrMicMotor2(); // Обработка прерывания по микрику на пине для мотора2
void isrMicMotor3(); // Обработка прерывания по микрику на пине для мотора3

// struct motorStruct // Структура для локального управления и сбора данных по моторам
// {
//     int status;        // Передаются импульсы на мотор или нет в данный момент, вращается или нет
//     int position;      // Текущая позиция в импульсах
//     int destination;   // Цель назначение в позиции в импульсах
//     int dir;           // Направление вращения мотора 1 - по часовой 0 - против часовой
//     int dir_port;      // Пин определяющий направление вращения
//     uint16_t dir_pin;  // Пин определяющий направление вращения
//     int step_port;     // Пин определяющий импульс
//     uint16_t step_pin; // Пин определяющий импульс
//     int micric_port;   // Пин определяющий концевик
//     int micric_pin;    // Пин определяющий концевик
// };
// struct motorStruct motor[4]; // Все локальные данные по моторам

// Функция устанавлявающая скорость вращения на ВСЕХ моторах, задается в оборотах за секунду rps
void setSpeedMotor(float _speed)
{
    // printf("===========================================setSpeedMotor= %f rps \n", _speed);

    // Скорость в оборотах их умножаем на градусы и делим на градус на 1 шаг получаем нужное число полных шагов для такой скорости за секунду
    // Умножаем на передаточное число редуктора и микрошаг
    float step_za_sec = abs(_speed) * (360 / STEPMOTOR) * REDUKTOR * MICROSTEP;
    // printf("step_za_sec= %f \n", (float)step_za_sec);

    // Микросекунды в секунде делим на число шагов которые надо успеть сделать за секунду и делим на микрошаги делим на микросекунды за 1 шаг с учетом предделителя таймера
    timeingStep = (float)1000000 / step_za_sec; // Таймер по 1 микросекунде
    Set_Timer7_Period(timeingStep);             //  Устаналиваем время на таймере
    // printf("timeingStep= %i microsecond \n", timeingStep);
    //  delay(100);
    HAL_Delay(100); // Пауза 100 миллисекунд.
}

// Функция инциализации моторов
void initMotor()
{
    //**************************************************************   Мотор на колеса
    // microStep = MICROSTEP; // Так распаяно на плате для драйверов 2208 и 2209
    // printf("Init StepMotor. Set microstep = %i \n", microStep);

    // Установка пина разрешающего работу драйвероы 0- Разрешена работа 1- запрещена работа драйвера
    HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.

    motor[0].step_port = 3; // Port C Step_Motor0_GPIO_Port;
    motor[0].step_pin = Step_Motor0_Pin;
    motor[0].dir_port = 3; // Dir_Motor0_GPIO_Port;
    motor[0].dir_pin = Dir_Motor0_Pin;

    motor[1].step_port = 2; // Port B Step_Motor1_GPIO_Port;
    motor[1].step_pin = Step_Motor1_Pin;
    motor[1].dir_port = 2; // Dir_Motor1_GPIO_Port;
    motor[1].dir_pin = Dir_Motor1_Pin;

    motor[2].step_port = 2; // Step_Motor2_GPIO_Port;
    motor[2].step_pin = Step_Motor2_Pin;
    motor[2].dir_port = 2; // Dir_Motor2_GPIO_Port;
    motor[2].dir_pin = Dir_Motor2_Pin;

    motor[3].step_port = 2; // Step_Motor3_GPIO_Port;
    motor[3].step_pin = Step_Motor3_Pin;
    motor[3].dir_port = 2; // Dir_Motor3_GPIO_Port;
    motor[3].dir_pin = Dir_Motor3_Pin;

    motor[0].status = 0; // Флаг ставим что мотор не работает, просто запрещаем делать импульсы
    motor[1].status = 0; // Флаг ставим что мотор не работает, просто запрещаем делать импульсы
    motor[2].status = 0; // Флаг ставим что мотор не работает, просто запрещаем делать импульсы
    motor[3].status = 0; // Флаг ставим что мотор не работает, просто запрещаем делать импульсы
}

// Функция для изменения периода таймера TIM7
void Set_Timer7_Period(uint32_t new_period)
{
    HAL_TIM_Base_Stop_IT(&htim7);                 // Останавливаем таймер перед изменением
    __HAL_TIM_SET_AUTORELOAD(&htim7, new_period); // Задаем новый период таймера (в пересчете на такты таймера)
    HAL_TIM_Base_Start_IT(&htim7);                // Запускаем таймер с новым периодом
}

void timer7() // Обработчик прерывания таймера TIM7
{
    // digitalWrite(2, 1);
    //  countPulse++;

    for (int i = 0; i < 4; i++)
    {
        if (motor[i].status)
        {
            if (motor[i].position == motor[i].destination && statusTestMotor == false) // Статус statusTestMotor только для отладки чтобы включить моторы на постоянное вращение
            {
                motor[i].status = 0;
            }
            else
            {
                if (motor[i].step_port == 1)
                    myPort = GPIOA;
                if (motor[i].step_port == 2)
                    myPort = GPIOB;
                if (motor[i].step_port == 3)
                    myPort = GPIOC;

                // digitalWrite(motor[i].step_pin, 1);                              // Если флаг вращения моторов включен тогда делаем импульс
                HAL_GPIO_WritePin(myPort, motor[i].step_pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.

                (motor[i].dir == 1) ? motor[i].position++ : motor[i].position--; // Если считаем шаги
            }
        }
    }

    // delayMicroseconds(1); // На stm32 не нужно и так 5 микросекунд выходит импульс

    for (int i = 0; i < 4; i++)
    {
        if (motor[i].step_port == 1)
            myPort = GPIOA;
        if (motor[i].step_port == 2)
            myPort = GPIOB;
        if (motor[i].step_port == 3)
            myPort = GPIOC;

        // digitalWrite(motor[i].step_pin, 0); // Отключаем импульс, делаем всегда на всех пинах без проверки включали ли его
        HAL_GPIO_WritePin(myPort, motor[i].step_pin, GPIO_PIN_RESET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
    }

    // timerAlarmWrite(timer1, timeingStep, true); // Какой таймер, до скольки считаем , сбрасываем ли счетчик при срабатывании. Значение посчитали когда скороть вращения расчитывали
    //  digitalWrite(2, 0);
}

// Запуск моторов на тест
void testMotorRun()
{
    HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_RESET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
    // digitalWrite(PIN_Motor_En, 0); // Включаем драйвера
    statusTestMotor = 1; // Статус теста мотора Включаем что тест

    motor[0].status = 1;
    motor[1].status = 1;
    motor[2].status = 1;
    motor[3].status = 1;
    // Serial.println("testMotorRun...");
    while (1)
        ;
}

// Остановка моторов после теста
void testMotorStop()
{
    motor[0].status = 0;
    motor[1].status = 0;
    motor[2].status = 0;
    motor[3].status = 0;
    // digitalWrite(PIN_Motor_En, 1); // Выключаем драйвера
    HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
    // Serial.println("testMotorStop...");
}

// Отключение моторов в простое
void disableMotor()
{
    if (motor[0].status == 0 && // Если все статусы выключены, значит моторы не двигаются и можно выключать
        motor[1].status == 0 &&
        motor[2].status == 0 &&
        motor[3].status == 0)
    {
        HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
        // digitalWrite(PIN_Motor_En, 1); // Выключаем драйвера
    }
}

// Пересчет градусов в импульсы
int32_t getPulse(float _angle)
{
    return round(_angle / sector);
}

// Пересчет импульсов в градусы
float getAngle(int32_t _pulse)
{
    return _pulse * sector;
}

// Установка мотора в нужное положение в градусах локальной системы
void setMotorAngle(int num, float _angle)
{
    if (_angle < 0)
        _angle = 0; // Защита от отрицательного градуса угла
    if (_angle > 180)
        _angle = 180;                                  // Защита от отклонения больше предела
    motor[num].destination = getPulse(_angle);         // Получаем в какую позицию должен встать мотор наиболее близкую к требуемому градусу
    if (motor[num].position == motor[num].destination) // Если текущая позиция и так тавна цели то ничего не делаем и выходим из функции
        return;

    // digitalWrite(PIN_Motor_En, 0); // Включаем драйвера
    HAL_GPIO_WritePin(En_Motor_GPIO_Port, En_Motor_Pin, GPIO_PIN_RESET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.

    printf("set num= %i ", num);
    printf("pos= %i ", motor[num].position);
    printf("dest= %i \n", motor[num].destination);

    if (motor[num].step_port == 1)
        myPort = GPIOA;
    if (motor[num].step_port == 2)
        myPort = GPIOB;
    if (motor[num].step_port == 3)
        myPort = GPIOC;

    if (motor[num].position < motor[num].destination) // Если цель бпльше то вращение по часовой 1
    {
        // digitalWrite(motor[num].dir_pin, 1);
        HAL_GPIO_WritePin(myPort, motor[num].dir_pin, GPIO_PIN_SET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
        motor[num].dir = 1;
    }
    if (motor[num].position > motor[num].destination) // Если цель меньше то вращение против часовой 0
    {
        // digitalWrite(motor[num].dir_pin, 0);
        HAL_GPIO_WritePin(myPort, motor[num].dir_pin, GPIO_PIN_RESET); // Установить пин HGH GPIO_PIN_SET — установить HIGH,  GPIO_PIN_RESET — установить LOW.
        motor[num].dir = 0;
    }
    // printf("dir_pin motor %i = %i \n", num, motor[num].dir_pin);
    motor[num].status = 1; // Включаем сам мотор
}

// Обработка прерывания по микрику на пине для мотора0
void isrMicMotor0()
{
    motor[0].status = false;
}

// Обработка прерывания по микрику на пине для мотора1
void isrMicMotor1()
{
    motor[1].status = false;
}

// Обработка прерывания по микрику на пине для мотора2
void isrMicMotor2()
{
    motor[2].status = false;
}

// Обработка прерывания по микрику на пине для мотора3
void isrMicMotor3()
{
    motor[3].status = false;
}

// Функция установки в ноль всех моторов
void setZeroMotor()
{
    setSpeedMotor(0.5); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
    // Serial.println(String(micros()) + " Start setZeroMotor ...");
    for (int i = 0; i < 4; i++) // Сначала отводим немного на случай если уже в нуле
    {
        setMotorAngle(i, 15);
    }
    HAL_Delay(500);
    setSpeedMotor(0.2); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
    for (int i = 0; i < 4; i++)
    {
        motor[i].position = 200 * REDUKTOR * MICROSTEP; // Устанавливаем всем максимульную позицию 200 шагов пополам (всего 400 так как мотор 0,9) на редуктор и на микрошаг
        setMotorAngle(i, 0);
    }
    HAL_Delay(5000);            // Время что-бы успел доехать до концевика
    for (int i = 0; i < 4; i++) // По итогу обнуляем позицию и назначение
    {
        motor[i].position = 0;    // Устанавливаем начальную позицию
        motor[i].destination = 0; // Устанавливаем начальную позицию
    }
    setSpeedMotor(SPEED); // Устанавливаем скорость вращения моторов и в дальнейшем только флагами включаем или отключаем вращение
    // Serial.println(String(micros()) + " End setZeroMotor ...");
}

#endif
