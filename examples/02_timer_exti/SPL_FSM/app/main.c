/**
 * @brief         Timer 1ms系统节拍 + 非阻塞LED闪烁 + 定时扫描型按键有限状态机
 * @par firmware: STM32F103
 * SPL
 * @date          2026-10-20
 * @note          LED：delay阻塞翻转->基于节拍的500ms非阻塞闪烁。
 */
#include "stm32f10x.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"
#include "app_config.h"

/* LED 非阻塞闪烁周期（ms） */
#define LED_BLINK_PERIOD_MS    500U

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

        /* 按键状态机扫描 */
        if ((uint32_t)(now - key_last_scan_tick) >= KEY_SCAN_PERIOD_MS)
        {
            key_last_scan_tick = now;
            key_event_t key_event;
            key_event = KEY_Process();

            switch (key_event)
            {
                case KEY_EVENT_SINGLE_CLICK:
                {
                    /* 短按：翻转 LED，并重置闪烁节拍 */
                    LED_Toggle();
                    led_last_tick = now;
                }
                break;

                case KEY_EVENT_DOUBLE_CLICK:
                {
                    /* 示例：双击进入某种模式 */
                }
                break;
                
                case KEY_EVENT_LONG_START:
                {
                    /* 长按：示例，可自定义功能 */
                }
                break;

                case KEY_EVENT_LONG_REPEAT:
                {
                    /* 长按重复：示例，可自定义功能 */
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
