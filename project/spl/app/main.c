/**
 * @file    main.c
 * @brief   Day 2：EXTI 按键 + Timer 1ms 系统节拍 + 非阻塞 LED 闪烁
 * @note    承接 Day 1（feature/drv-gpio）的分层约定：
 *          - main.c 只调用 bsp/driver 接口，不直接操作寄存器；
 *          - 共享变量封装在各模块内部，通过接口读取（volatile + getter）；
 *          - 时间相关变量统一使用 uint32_t；
 *          - 超时判断使用差值写法，正确处理无符号溢出回绕：
 *              推荐：if ((uint32_t)(now - last) >= timeout)
 *              不推荐：if (now >= last + timeout)
 *          Day 1 -> Day 2 变化：
 *          - 按键由主循环轮询改为 EXTI 下降沿中断 + 事件计数；
 *          - LED 由 delay 阻塞翻转改为基于 1ms 节拍的 500ms 非阻塞闪烁。
 */

#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"
#include <stdint.h>

int main(void)
{
    /* 配置 NVIC 分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    /* 初始化外设 */
    bsp_led_init();
    bsp_key_init();
    drv_timer_tick_init();

    /* 仅主循环使用的变量，不需要 volatile */
    uint32_t led_last_tick = 0;
    uint32_t key_event_count_last = 0;

    while (1)
    {
        /*
         * 读取共享变量时，先取局部快照，
         * 避免一次逻辑中多次读取到不同值。
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

            /*
             * 按键事件发生后立即翻转 LED，
             * 并重置闪烁节拍。
             */
            bsp_led_toggle();
            led_last_tick = now;
        }
    }
}
