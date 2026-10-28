#include "bsp_key.h"
#include "drv_gpio.h"
#include "drv_timer_tick.h"
#include "app_config.h"

/* 按键事件计数器 */
static volatile uint32_t s_key_event_count = 0;
static volatile uint32_t s_key_last_press_tick = 0;

/* 把按键对应的 GPIO 配置成外部中断输入，
并允许 CPU 响应按键中断。 */
void KEY_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* GPIO 输入初始化（内部上拉） */
    drv_gpio_input_init(KEY_GPIO_PORT,
                        KEY_GPIO_CLK,
                        KEY_GPIO_PIN,
                        KEY_INTPUT_PULL);

    /* 使能 AFIO 时钟，EXTI 线映射依赖 AFIO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* 将 PA0 映射到 EXTI Line0 */
    GPIO_EXTILineConfig(KEY_EXTI_PORT_SOURCE, KEY_EXTI_PIN_SOURCE);

    /* 配置 EXTI：下降沿触发中断 */
    EXTI_InitStruct.EXTI_Line    = KEY_EXTI_LINE;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* 配置 NVIC */
    NVIC_InitStruct.NVIC_IRQChannel                   = KEY_EXTI_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
}

/* 封装接口， 后续可扩展
允许外部通过接口读取 按键事件计数器 */
uint32_t KEY_GetEventCount(void)
{
    return s_key_event_count;
}

/* 读取按键状态 */
key_level_t KEY_Read(void)
{
    uint8_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    if (level == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED_LEVEL;
    }
    else
    {
        return KEY_RELEASED_LEVEL;
    }
}

void KEY_EXTI_IRQHandler(void)
{
    if (EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(KEY_EXTI_LINE);

        /*
         * 中断中只记录事件次数，
         * 不做按键消抖、业务处理、打印等复杂操作。
         * 中断消抖锁定机制，
         * 若距离上次有效触发不足 50ms，视为物理机械抖动，直接滤除。
         */
        uint32_t now = drv_timer_tick_get();
        if ((uint32_t)(now - s_key_last_press_tick) >= KEY_DEBOUNCE_TIME_MS)
        {
            s_key_last_press_tick = now;
            s_key_event_count++;
        }
    }
}
