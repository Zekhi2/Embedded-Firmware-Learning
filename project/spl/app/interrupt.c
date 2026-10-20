/**
 * @file    interrupt.c
 * @brief   中断入口转发（SPL 版本，Day 2）
 * @note    这里不写具体逻辑，只负责转发给对应模块处理函数。
 */

#include "stm32f10x.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

/*=====================================================
 * 中断入口函数
 *
 * 这里不写具体逻辑，
 * 只负责转发给对应模块处理。
 *=====================================================*/

void EXTI0_IRQHandler(void)
{
    bsp_key_exti_irq_handler();
}

void TIM3_IRQHandler(void)
{
    drv_timer_tick_irq_handler();
}
