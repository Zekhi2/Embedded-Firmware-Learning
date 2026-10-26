/**
 * @file    drv_timer_tick.c
 * @brief   TIM3 产生 1ms 系统节拍
 * @note    中断安全原则：
 *          - 中断中只做节拍累加这一件事；
 *          - 更新中断标志必须清除；
 *          - 与主循环共享的节拍变量使用 volatile；
 *          - 时间相关变量统一使用 uint32_t。
 */
#include "drv_timer_tick.h"
#include "app_config.h"

/*
 * 定义系统节拍计数器，只能在当前文件中可见。
 * 该变量在 Timer 中断中修改，在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_system_tick_ms = 0;

void drv_timer_tick_init(void)
{
    /* SPL库 结构体配置定时器基本参数 */
    TIM_TimeBaseInitTypeDef TIM_InitStruct;
    /* SPL库 结构体配置中断控制器 */
    NVIC_InitTypeDef NVIC_InitStruct;

    /* 打开定时器时钟 */
    RCC_APB1PeriphClockCmd(SYSTEM_TICK_TIM_CLK, ENABLE);

    /* 配置定时器基本参数 */
    TIM_InitStruct.TIM_Prescaler         = SYSTEM_TICK_TIM_PSC;
    TIM_InitStruct.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_InitStruct.TIM_Period            = SYSTEM_TICK_TIM_ARR;
    TIM_InitStruct.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_InitStruct.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(SYSTEM_TICK_TIM, &TIM_InitStruct);

    /* 使能自动重装载预装载 */
    TIM_ARRPreloadConfig(SYSTEM_TICK_TIM, ENABLE);

    /* 清除更新标志 */
    TIM_ClearFlag(SYSTEM_TICK_TIM, TIM_FLAG_Update);

    /* 使能更新中断 */
    TIM_ITConfig(SYSTEM_TICK_TIM, TIM_IT_Update, ENABLE);

    /* 配置 NVIC */
    NVIC_InitStruct.NVIC_IRQChannel                   = SYSTEM_TICK_TIM_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 启动定时器 */
    TIM_Cmd(SYSTEM_TICK_TIM, ENABLE);
}

/* 封装接口， 后续可扩展
允许外部通过接口读取 系统节拍计数器 */
uint32_t drv_timer_tick_get(void)
{
    return s_system_tick_ms;
}

void drv_timer_tick_irq_handler(void)
{
    if (TIM_GetITStatus(SYSTEM_TICK_TIM, TIM_IT_Update) != RESET)
    {
        /* 清除中断标志 */
        TIM_ClearITPendingBit(SYSTEM_TICK_TIM, TIM_IT_Update);

        /*
         * 系统节拍累加。中断中只做这一件事。
         */
        s_system_tick_ms++;
    }
}