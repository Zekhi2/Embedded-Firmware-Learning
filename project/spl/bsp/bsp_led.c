/**
 * @file    bsp_led.c
 * @brief   LED 板级控制实现（SPL 版本，Day 2）
 * @note    与 Day 1（feature/drv-gpio）一致：
 *          - 通过 LED_ACTIVE_LOW 宏屏蔽高/低电平点亮差异；
 *          - 内部维护 s_led_state 软件状态镜像；
 *          - main.c 不直接操作寄存器。
 */

#include "bsp_led.h"
#include "app_config.h"

static uint8_t s_led_state = 0;

static void led_write(uint8_t on)
{
#if (LED_ACTIVE_LOW == 1)
    if (on == 1)
    {
        /* 低电平点亮 */
        GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    }
    else
    {
        /* 高电平熄灭 */
        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    }
#else
    if (on == 1)
    {
        /* 高电平点亮 */
        GPIO_SetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    }
    else
    {
        /* 低电平熄灭 */
        GPIO_ResetBits(LED_GPIO_PORT, LED_GPIO_PIN);
    }
#endif
}

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(LED_GPIO_CLK, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = LED_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    s_led_state = 0;
    led_write(0);
}

void bsp_led_on(void)
{
    s_led_state = 1;
    led_write(1);
}

void bsp_led_off(void)
{
    s_led_state = 0;
    led_write(0);
}

void bsp_led_toggle(void)
{
    s_led_state ^= 1;
    led_write(s_led_state);
}
