#include "drv_timer_tick.h"
#include "tim.h"

static volatile uint32_t s_system_tick_ms = 0;

void drv_timer_tick_init(void)
{
    MX_TIM3_Init();
    HAL_TIM_Base_Start_IT(&htim3);
}

uint32_t drv_timer_tick_get(void)
{
    return s_system_tick_ms;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        s_system_tick_ms++;
    }
}
