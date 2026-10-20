#include "bsp_key.h"
#include "drv_gpio.h"
#include "app_config.h"

/*
 * 该变量在 EXTI 中断中修改，
 * 在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_key_event_count = 0;

void KEY_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 复用 Day 1 的 GPIO 输入初始化（内部上拉） */
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

uint32_t KEY_GetEventCount(void)
{
    return s_key_event_count;
}

key_state_t KEY_Read(void)
{
    uint8_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    if (level == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_RELEASED;
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
         */
        s_key_event_count++;
    }
}
