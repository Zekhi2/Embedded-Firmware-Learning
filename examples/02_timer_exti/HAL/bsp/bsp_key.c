#include "bsp_key.h"
#include "drv_timer_tick.h"
#include "app_config.h"

/* 按键事件计数器与消抖锁定节拍 */
static volatile uint32_t s_key_event_count = 0;
static volatile uint32_t s_key_last_press_tick = 0;

void KEY_Init(void)
{
    /* 引脚模式、内部上拉及 EXTI 中断均已由 MX_GPIO_Init() 配置 */
    s_key_event_count = 0;
    s_key_last_press_tick = 0;
}

uint32_t KEY_GetEventCount(void)
{
    return s_key_event_count;
}

key_state_t KEY_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }

    return KEY_RELEASED;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_PIN)
    {
        /*
         * 中断中只记录事件次数，
         * 中断消抖锁定机制：若距离上次有效触发不足 KEY_DEBOUNCE_TIME_MS，直接滤除。
         */
        uint32_t now = drv_timer_tick_get();
        if ((uint32_t)(now - s_key_last_press_tick) >= KEY_DEBOUNCE_TIME_MS)
        {
            s_key_last_press_tick = now;
            s_key_event_count++;
        }
    }
}
