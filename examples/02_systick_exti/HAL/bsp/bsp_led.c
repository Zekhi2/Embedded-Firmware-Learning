#include "bsp_led.h"
#include "app_config.h"

static uint8_t s_led_state = 0;

static void led_write(uint8_t on)
{
#if (LED_ACTIVE_LOW == 1)
    if (on == 1)
    {
        /* 低电平点亮 */
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
    }
    else
    {
        /* 高电平熄灭 */
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
    }
#else
    if (on == 1)
    {
        /* 高电平点亮 */
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);
    }
    else
    {
        /* 低电平熄灭 */
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_RESET);
    }
#endif
}

void bsp_led_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* 初始熄灭 */
    HAL_GPIO_WritePin(LED_GPIO_PORT, LED_GPIO_PIN, GPIO_PIN_SET);

    GPIO_InitStruct.Pin   = LED_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_PORT, &GPIO_InitStruct);

    s_led_state = 0;
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
