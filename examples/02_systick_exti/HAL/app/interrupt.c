#include "app_config.h"
#include "drv_timer_tick.h"

/*=====================================================
 * 中断入口函数
 *
 * 这里不写具体逻辑，
 * 只负责调用 HAL 中断处理函数。
 *=====================================================*/

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_GPIO_PIN);
}

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timer_tick_htim);
}
