#ifndef __CODE_H__
#define __CODE_H__

class CCode
{
private:
    /* data */
public:
    void main();
    void timer6Run(); // Обработчик прерывания таймера 0 по совпадению A 	1 раз в 1 милисекунду
};

void CCode::main()
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8); // Инвертирование состояния выхода.
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
    HAL_Delay(500); // Пауза 500 миллисекунд.
}

void CCode::timer6Run() // Обработчик прерывания таймера 0 по совпадению A 	1 раз в 1 милисекунду
{
//   count_timer_10millisec++;
//   count_timer_50millisec++;
//   count_timer_1sec++;
//   // Serial.println(".");
//   //  каждые 10 милисекунд
//   if (count_timer_10millisec >= 10)
//   {
//     count_timer_10millisec = 0;
//     flag_timer_10millisec = true;
//   }
//   // каждые 50 милисекунд
//   if (count_timer_50millisec >= 50)
//   {
//     count_timer_50millisec = 0;
//     flag_timer_50millisec = true;
//   }
//   // 1 seconds
//   if (count_timer_1sec >= 1000)
//   {
//     count_timer_1sec = 0;
//     flag_timer_1sec = true;
//   }
}

#endif /*__CODE_H__*/