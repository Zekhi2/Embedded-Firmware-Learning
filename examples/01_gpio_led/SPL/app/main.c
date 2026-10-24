/**
 * @brief         读取按键，按键控制LED
 * @par firmware: STM32F103
 * SPL
 * @par compiler: Keil MDK
 * @date          2026-10-20
 * @version       V1.0
 * @note          备注
 */
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"

int main(void)
{
    LED_Init();
    KEY_Init();

    while (1)
    {
        if (KEY_Read() == KEY_PRESSED)
        {
            LED_On();
        }
        else
        {
            LED_Off();
        }
    }
}