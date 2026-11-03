/**
 * @brief         Timer 1ms系统节拍 + EXTI按键中断 + 非阻塞LED闪烁
 * @par firmware: STM32F103 HAL (STM32CubeMX)
 * @date          2026-10-25
 * @note          按键：EXTI下降沿中断 + 消抖锁定事件计数；
 *                LED：基于节拍的500ms非阻塞闪烁。
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
         * 按键事件处理：产生按键事件时立即翻转 LED 并重置闪烁节拍
         */
        if (key_event_count_snapshot != key_event_count_last)
        {
            key_event_count_last = key_event_count_snapshot;

            LED_Toggle();
            led_last_tick = now;
        }

        /*
         * 非阻塞 LED 闪烁：500ms 一次
         */
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
