#ifndef CONFIG_H
#define CONFIG_H

#define Analiz1_Pin GPIO_PIN_13
#define Analiz1_GPIO_Port GPIOC

#define Analiz2_Pin GPIO_PIN_10
#define Analiz2_GPIO_Port GPIOC

#define Led2_Pin GPIO_PIN_6
#define Led2_GPIO_Port GPIOC

#define Led1_Pin GPIO_PIN_7
#define Led1_GPIO_Port GPIOC

#define Step_Motor0_Pin GPIO_PIN_1
#define Step_Motor0_GPIO_Port GPIOC
#define Dir_Motor0_Pin GPIO_PIN_2
#define Dir_Motor0_GPIO_Port GPIOC

#define Step_Motor1_Pin GPIO_PIN_1
#define Step_Motor1_GPIO_Port GPIOB
#define Dir_Motor1_Pin GPIO_PIN_2
#define Dir_Motor1_GPIO_Port GPIOB

#define Step_Motor2_Pin GPIO_PIN_10
#define Step_Motor2_GPIO_Port GPIOB
#define Dir_Motor2_Pin GPIO_PIN_11
#define Dir_Motor2_GPIO_Port GPIOB

#define Dir_Motor3_Pin GPIO_PIN_14
#define Step_Motor3_Pin GPIO_PIN_15
#define Step_Motor3_GPIO_Port GPIOB
#define Dir_Motor3_GPIO_Port GPIOB

#define En_Motor_Pin GPIO_PIN_8
#define En_Motor_GPIO_Port GPIOA

#define micMotor0_Pin GPIO_PIN_3
#define micMotor0_GPIO_Port GPIOC
#define micMotor0_EXTI_IRQn EXTI2_3_IRQn

#define micMotor1_Pin GPIO_PIN_0
#define micMotor1_GPIO_Port GPIOB
#define micMotor1_EXTI_IRQn EXTI0_1_IRQn

#define micMotor2_Pin GPIO_PIN_12
#define micMotor2_GPIO_Port GPIOB
#define micMotor2_EXTI_IRQn EXTI4_15_IRQn

#define micMotor3_Pin GPIO_PIN_13
#define micMotor3_GPIO_Port GPIOB
#define micMotor3_EXTI_IRQn EXTI4_15_IRQn



enum codeOperation // коды операций запросов по UART
{
    No,  // нет операций
    Stop, // стоп измерения
    Continuous, // Непрерывное измерение
    Single, // Одиночное измерние
    Address, // Установка адреса
    Cache, // запрос кэш тз датчика
    Status, // Включение отключение адтчика
    Broadcast, // Запрос на измерение ко всем датчикам на линии, потом считать из кэша
    StartPoint, // Установка точки отсчета
    TimeInterval, // Установка интревала вывода при постоянных измерениях. Возможно это усреднение значений ???
    SetRange,
    SetResolution
};

//*********************************************************************
struct motorStruct // Структура для локального управления и сбора данных по моторам
{
    int status;        // Передаются импульсы на мотор или нет в данный момент, вращается или нет
    int position;      // Текущая позиция в импульсах
    int destination;   // Цель назначение в позиции в импульсах
    int dir;           // Направление вращения мотора 1 - по часовой 0 - против часовой
    int dir_port;      // Пин определяющий направление вращения
    uint16_t dir_pin;  // Пин определяющий направление вращения
    int step_port;     // Пин определяющий импульс
    uint16_t step_pin; // Пин определяющий импульс
    int micric_port;   // Пин определяющий концевик
    int micric_pin;    // Пин определяющий концевик
};
struct motorStruct motor[4]; // Все локальные данные по моторам

//*********************************************************************
struct SControlLaser
{
  uint32_t mode; // Текущий режим работы 0 - отключить датчики 1 - режим одновременного измерения
};
struct SControlMotor
{
  uint32_t mode;    // Текущий режим работы 0 - режим колибровки концевиков 1 - обычное управление моторами по углу
  float angle[4];       // Углы в которые нужно повернультя в локальной системе
  int32_t numPillar[4]; // Номер столба до которого измеряем расстояние
};

// Структура получаемых данных от Data к контроллеру Modul
struct Struct_Data2Modul
{
  uint32_t id;            // Номер команды по порядку
  struct SControlMotor controlMotor; // Управление моторами
  struct SControlLaser controlLaser; // Управление лазерами
  uint32_t cheksum;       // Контрольная сумма данных в структуре
};

struct Struct_Data2Modul Data2Modul_receive; // Экземпляр структуры получаемых данных

//const int size_structura_receive = sizeof(Data2Modul_receive); // Размер структуры с данными которые получаем

//*********************************************************************
// Структура по состоянию лидаров которая передается на верхний уровень
struct SLaserSend
{
  uint32_t status;        // Статус датчика или ошибки
  float distance;         // Последнее измерение
  uint32_t signalQuality; // Качество сигнала
  float angle;            // Положение при последнем измерении
  int32_t numPillar;     // Номер столба до которого измерили расстояние
};
struct SLaserSend laser[4]; // Все локальные данные по лазерам

struct SMotorSend // Структура которая передается на верхний уровень
{
  int32_t status;    // Передаются импульсы на мотор или нет в данный момент, вращается или нет
  float position;    // Текущая позиция в градусах
  float destination; // Цель назначение в позиции в градусах
};
// Структура по обратной связи по обмену по шине SPI
struct SSpi
{
  uint32_t all;
  uint32_t bed;
};
struct SSpi spi; // Переменная где все данные по обмену

// Структура в которой все главные переменные передаюся на высокий уровень от Modul к Data
struct Struct_Modul2Data
{
  uint32_t id; // id команды

  uint32_t pinMotorEn;      // Стутус пина управления драйвером моторов, включен драйвер или нет
  struct SMotorSend motor[4];          // Структура по состоянию моторов
  struct SLaserSend laser[4];          // Структура по состоянию лазеров
  uint32_t statusDataLaser; // Статус обновления данных с лазерных датчиков
  uint32_t micric[4];           // Структура по состоянию концевиков
  struct SSpi spi;                     // Структура по состоянию обмена по шине SPI

  uint32_t cheksum; // Контрольная сумма данных в структуре
};

struct Struct_Modul2Data Modul2Data_send;                                                                                             // Тут все переменные его характеризующие на низком уровне
// const int size_structura_send = sizeof(Modul2Data_send);                                                                       // Размер структуры с данными которые передаем
//const uint16_t max_size_stuct = (size_structura_receive < size_structura_send) ? size_structura_send : size_structura_receive; // Какая из структур больше

#endif
