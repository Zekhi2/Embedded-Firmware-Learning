/**
 * @brief         Timer 1ms系统节拍 + 非阻塞LED闪烁 + EXTI按键有限状态机
 * @par firmware: STM32F103
 * SPL
 * @date          2026-10-20
 * @note          LED：delay阻塞翻转->基于节拍的500ms非阻塞闪烁。
 *                KEY：EXTI + bool flag 边沿标记按键有限状态机。
 */
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

int main(void)
{
    uint32_t key_last_scan_tick = 0;
    uint32_t led_last_tick      = 0;

    /* 配置 NVIC 分组 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    drv_timer_tick_init();
    LED_Init();
    KEY_Init();

    while (1)
    {
        /*
         * 读取共享变量时，先取局部快照，
         * 避免一次逻辑中多次读取到不同值。
         */
        uint32_t now = drv_timer_tick_get();
        bool need_key_scan = false;

        /* 边沿标志触发扫描 */
        if (KEY_TakeEdgeFlag() == true)
        {
            need_key_scan = true;
        }

        /* 周期扫描 */
        if ((uint32_t)(now - key_last_scan_tick) >= KEY_SCAN_PERIOD_MS)
        {
            need_key_scan = true;
        }

        /* 执行按键状态机扫描 */
        if (need_key_scan == true)
        {
            key_event_t key_event;

            key_last_scan_tick = now;

            key_event = KEY_Process();

            switch (key_event)
            {
                case KEY_EVENT_SINGLE_CLICK:
                {
                    /*
                     * 单击：
                     * 翻转 LED，并重置 LED 自动闪烁节拍。
                     */
                    LED_Toggle();
                    led_last_tick = now;
                }
                break;

                case KEY_EVENT_DOUBLE_CLICK:
                {
                    /*
                     * 双击：
                     * 示例占位。
                     *
                     * 可以在这里进入某种模式、切换状态等。
                     */
                }
                break;

                case KEY_EVENT_LONG_START:
                {
                    /*
                     * 长按开始：
                     * 示例占位。
                     */
                }
                break;

                case KEY_EVENT_LONG_REPEAT:
                {
                    /*
                     * 长按重复：
                     * 示例占位。
                     */
                }
                break;

                default:
                break;
            }
        }
        
        /* 非阻塞 LED 闪烁：500ms 一次 */
        if ((uint32_t)(now - led_last_tick) >= LED_BLINK_PERIOD_MS)
        {
            led_last_tick = now;
            LED_Toggle();
        }
    }
}
