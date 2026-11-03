/**
 * @brief         点亮LED
 * @par firmware: STM32F103
 * SPL
 * @par compiler: Keil MDK
 * @date          2026-10-20
 * @version       V1.0
 * @note          备注
 */
#include "stm32f10x.h"
#include "bsp_led.h"

int main(void)
{
    LED_Init();

    LED_On();

    while (1)
    {

    }
}