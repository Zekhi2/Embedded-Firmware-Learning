/* User/bsp/bsp_key.c */
#include "bsp_key.h"
#include "app_config.h"

void KEY_Init(void)
{
    /*
     * 输入模式 + 内部上拉 + 时钟已由 MX_GPIO_Init() 配置：
     *   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
     *   GPIO_InitStruct.Pull = GPIO_PULLUP;
     * 因此这里无需任何操作。保留函数仅为接口对称与后续扩展（如消抖）。
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