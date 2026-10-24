/**
 * @file    interrupt.c
 * @brief   中断入口转发（HAL 版）
 * @note    这里不写具体逻辑，只负责调用 HAL 中断处理函数。
 *          若工程由 CubeMX 重新生成，可将这两个函数搬入
 *          stm32f1xx_it.c 的 USER CODE 区域，功能等价。
 */
#include "main.h"
#include "app_config.h"
#include "drv_timer_tick.h"

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_PIN);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timer_tick_htim);
}
