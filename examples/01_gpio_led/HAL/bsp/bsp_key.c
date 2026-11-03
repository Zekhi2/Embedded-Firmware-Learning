/* User/bsp/bsp_key.c */
#include "bsp_key.h"
#include "app_config.h"

void KEY_Init(void)
{
    /*
     * 引脚模式 + 内部偏置 + 时钟已由 MX_GPIO_Init() 配置。
     */
}

key_state_t KEY_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }

    return KEY_RELEASED;
}
