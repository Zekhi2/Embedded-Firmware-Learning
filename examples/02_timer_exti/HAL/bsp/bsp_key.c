/* User/bsp/bsp_key.c */
#include "bsp_key.h"
#include "app_config.h"

/*
 * 该变量在 EXTI 回调中修改，
 * 在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_key_event_count = 0;

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /*
     * Day 2：按键由 Day 1 的轮询输入改为 EXTI 下降沿中断。
     * CubeMX 生成的 gpio.c 中 KEY 仍为普通输入模式，
     * 这里重新配置为 IT_FALLING（等效于 Mode = GPIO_MODE_IT_FALLING, Pull = GPIO_PULLUP）。
     */
    GPIO_InitStruct.Pin  = KEY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(KEY_EXTI_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(KEY_EXTI_IRQn);
}

key_state_t KEY_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }

    return KEY_RELEASED;
}

uint32_t KEY_GetEventCount(void)
{
    return s_key_event_count;
}

/*
 * HAL GPIO EXTI 回调。
 * 如果有多个 EXTI 引脚，可以在这里统一分发。
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_PIN)
    {
        /*
         * 中断中只记录事件次数，
         * 复杂业务放到主循环处理。
         */
        s_key_event_count++;
    }
}
