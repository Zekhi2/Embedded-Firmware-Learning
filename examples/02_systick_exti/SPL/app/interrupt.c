/**
 * @brief         中断入口转发
 * @par firmware: STM32F103
 * SPL
 * @par compiler: Keil MDK
 * @date          2026-10-20
 * @version       V1.0
 * @note          这里不写具体逻辑，只负责转发给对应模块处理函数。
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
