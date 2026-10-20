/**
 * @file    bsp_key.c
 * @brief   按键 EXTI 外部中断实现（SPL 版本，Day 2）
 * @note    相对 Day 1（feature/drv-gpio）的变化：
 *          - Day 1：GPIO 输入 + 主循环轮询读取电平；
 *          - Day 2：PA0 配置为 IPU 输入并映射到 EXTI Line0，
 *            下降沿触发中断，中断中只累加事件计数。
 *          中断安全原则：
 *          - 中断中只处理简单事件（计数），不做消抖/打印/业务；
 *          - 中断标志必须清除；
 *          - 与主循环共享的变量使用 volatile。
 */

#include "bsp_key.h"
#include "app_config.h"

/*
 * 该变量在 EXTI 中断中修改，
 * 在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_key_event_count = 0;

void bsp_key_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 使能 GPIOA 和 AFIO 时钟 */
    RCC_APB2PeriphClockCmd(KEY_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

    /* PA0 上拉输入，按键按下产生低电平 */
    GPIO_InitStruct.GPIO_Pin  = KEY_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);

    /* 将 PA0 映射到 EXTI Line0 */
    GPIO_EXTILineConfig(KEY_EXTI_PORT_SOURCE, KEY_EXTI_PIN_SOURCE);

    /* 配置 EXTI */
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

uint32_t bsp_key_get_event_count(void)
{
    return s_key_event_count;
}

void bsp_key_exti_irq_handler(void)
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
