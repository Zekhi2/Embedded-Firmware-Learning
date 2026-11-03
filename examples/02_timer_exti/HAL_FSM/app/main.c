/**
 * @brief         Timer 1ms系统节拍 + 非阻塞LED闪烁 + 按键有限状态机
 * @par firmware: STM32F103 HAL (STM32CubeMX)
 * @date          2026-10-25
 * @note          LED：基于节拍的500ms非阻塞闪烁。
 *                KEY：定时扫描型按键有限状态机处理。
 */
#include "main.h"
#include "gpio.h"
#include "app_config.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

void SystemClock_Config(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    drv_timer_tick_init();
    LED_Init();
    KEY_Init();

    uint32_t led_last_tick = 0;
    uint32_t key_last_scan_tick = 0;

    while (1)
    {
        uint32_t now = drv_timer_tick_get();

        /* 按键状态机周期扫描 */
        if ((uint32_t)(now - key_last_scan_tick) >= KEY_SCAN_PERIOD_MS)
        {
            key_last_scan_tick = now;
            key_event_t key_event = KEY_Process();

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
                    /* 双击示例，可扩展 */
                }
                break;

                case KEY_EVENT_LONG_START:
                {
                    /* 长按示例，可扩展 */
                }
                break;

                case KEY_EVENT_LONG_REPEAT:
                {
                    /* 长按连发示例，可扩展 */
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

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}
