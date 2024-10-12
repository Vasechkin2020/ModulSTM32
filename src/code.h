#ifndef __CODE_H__
#define __CODE_H__

#include <stddef.h>

class CCode
{
private:
    /* data */
public:
    void main();
};

void CCode::main()
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8); // Инвертирование состояния выхода.
    // HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10); // Инвертирование состояния выхода.
    HAL_Delay(500); // Пауза 500 миллисекунд.
}

#endif /*__CODE_H__*/