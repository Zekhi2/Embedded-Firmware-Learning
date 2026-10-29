/* User/bsp/bsp_led.c */
#include "bsp_led.h"
#include "app_config.h"

void LED_Init(void)
{
    /*
     * 引脚时钟 / 推挽输出 / 初始电平均已由 MX_GPIO_Init() 配置完成。
     * 但为了模块独立性，可以在这里配置而非用 cubemx配置，这也是一种方案。
     */
    LED_Off(); /* 保证上电状态，属重复配置 */
}

void LED_On(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_ON_LEVEL);
}

void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_OFF_LEVEL);
}

void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
}
