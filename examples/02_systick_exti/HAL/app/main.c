#include "app_config.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"
#include <stdint.h>

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();

    /*
     * 系统时钟配置。
     * 实际工程中通常由 CubeMX 生成。
     * 这里假设配置为 72MHz。
     */
    SystemClock_Config();

    bsp_led_init();
    bsp_key_init();
    drv_timer_tick_init();

    /* 仅主循环使用的变量，不需要 volatile */
    uint32_t led_last_tick = 0;
    uint32_t key_event_count_last = 0;

    while (1)
    {
        /*
         * 读取共享变量时，先取局部快照。
         */
        uint32_t now = drv_timer_tick_get();
        uint32_t key_event_count_snapshot = bsp_key_get_event_count();

        /*-------------------------------------------------
         * 1. 非阻塞 LED 闪烁：500ms 一次
         *------------------------------------------------*/
        if ((uint32_t)(now - led_last_tick) >= 500U)
        {
            led_last_tick = now;
            bsp_led_toggle();
        }

        /*-------------------------------------------------
         * 2. 按键事件处理
         *------------------------------------------------*/
        if (key_event_count_snapshot != key_event_count_last)
        {
            key_event_count_last = key_event_count_snapshot;

            bsp_led_toggle();
            led_last_tick = now;
        }
    }
}

/*
 * 实际工程中请使用 CubeMX 生成，
 * 或根据开发板时钟树自行配置。
 */
void SystemClock_Config(void)
{
    /* TODO: 配置系统时钟到 72MHz */
}
