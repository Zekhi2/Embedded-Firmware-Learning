/**
 * @brief         中断入口转发
 * @par firmware: STM32F103
 * SPL
 * @date          2026-10-20
 * @note          只做中断向量入口分发，不夹带任何应用逻辑。
                  保留 interrupt.c或使用官方 stm32f10x_it.c。
 */
#include "stm32f10x.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

void EXTI0_IRQHandler(void)
{
    KEY_EXTI_IRQHandler();
}

void TIM3_IRQHandler(void)
{
    drv_timer_tick_irq_handler();
}
