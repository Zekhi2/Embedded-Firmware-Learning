/* User/bsp/bsp_led.c */
#include "bsp_led.h"
#include "app_config.h"

#define LED_INACTIVE_LEVEL  ((GPIO_PinState)(1U - (uint32_t)LED_ACTIVE_LEVEL))

void LED_Init(void)
{
    /* 
     * 引脚时钟 / 推挽输出 / 初始电平均已由 MX_GPIO_Init() 配置完成。
     */
    LED_Off();
}

void LED_On(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_ACTIVE_LEVEL);
}

void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_INACTIVE_LEVEL);
}

void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}
