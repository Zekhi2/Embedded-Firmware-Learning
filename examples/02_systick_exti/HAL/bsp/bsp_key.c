#include "bsp_key.h"
#include "app_config.h"

/*
 * 该变量在 EXTI 回调中修改，
 * 在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_key_event_count = 0;

void bsp_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* 配置 PA0 为上拉输入，下降沿触发 EXTI */
    GPIO_InitStruct.Pin   = KEY_GPIO_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(KEY_EXTI_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(KEY_EXTI_IRQn);
}

uint32_t bsp_key_get_event_count(void)
{
    return s_key_event_count;
}

/*
 * HAL GPIO EXTI 回调。
 * 如果有多个 EXTI 引脚，可以在这里统一分发。
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_GPIO_PIN)
    {
        /*
         * 中断中只记录事件次数，
         * 复杂业务放到主循环处理。
         */
        s_key_event_count++;
    }
}
