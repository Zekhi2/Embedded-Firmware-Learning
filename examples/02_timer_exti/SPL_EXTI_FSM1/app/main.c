/**
 * @brief         Timer 1ms系统节拍 + EXTI按键中断 + 非阻塞LED闪烁
 * @par firmware: STM32F103
 * SPL
 * @date          2026-10-20
 * @note          LED：delay阻塞翻转->基于节拍的500ms非阻塞闪烁。
 */
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

int main(void)
{
    /* 配置 NVIC 分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    drv_timer_tick_init();
    LED_Init();
    KEY_Init();

    uint32_t led_last_tick = 0;
    uint32_t key_event_count_last = 0;

    while (1)
    {
        /*
         * 读取共享变量时，先取局部快照，
         * 避免一次逻辑中多次读取到不同值。
         */
        uint32_t now = drv_timer_tick_get();
        uint32_t key_event_count_snapshot = KEY_GetEventCount();

        /*
         * 按键事件处理
         */
        if (key_event_count_snapshot != key_event_count_last)
        {
            key_event_count_last = key_event_count_snapshot;

            /*
             * 按键事件发生后立即翻转 LED，
             * 并重置闪烁节拍。
             */
            LED_Toggle();
            led_last_tick = now;
        }
        /*
         * 非阻塞 LED 闪烁：500ms 一次
         */
        else if ((uint32_t)(now - led_last_tick) >= LED_BLINK_PERIOD_MS)
        {
            led_last_tick = now;
            LED_Toggle();
        }
    }
}
