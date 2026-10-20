#include "drv_timer_tick.h"

TIM_HandleTypeDef g_timer_tick_htim;

/*
 * 该变量在 Timer 回调中修改，
 * 在主循环中通过接口读取，
 * 所以必须使用 volatile。
 */
static volatile uint32_t s_system_tick_ms = 0;

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == SYSTEM_TICK_TIM_INSTANCE)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();

        HAL_NVIC_SetPriority(SYSTEM_TICK_TIM_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SYSTEM_TICK_TIM_IRQn);
    }
}

void drv_timer_tick_init(void)
{
    g_timer_tick_htim.Instance               = SYSTEM_TICK_TIM_INSTANCE;
    g_timer_tick_htim.Init.Prescaler         = SYSTEM_TICK_TIM_PSC;
    g_timer_tick_htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
    g_timer_tick_htim.Init.Period            = SYSTEM_TICK_TIM_ARR;
    g_timer_tick_htim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    g_timer_tick_htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_Base_Init(&g_timer_tick_htim);

    HAL_TIM_Base_Start_IT(&g_timer_tick_htim);
}

uint32_t drv_timer_tick_get(void)
{
    return s_system_tick_ms;
}

/*
 * HAL Timer 更新中断回调。
 * 如果有多个定时器，需要根据 Instance 分发。
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == SYSTEM_TICK_TIM_INSTANCE)
    {
        /*
         * 系统节拍累加。
         * 中断中只做这一件事。
         */
        s_system_tick_ms++;
    }
}
