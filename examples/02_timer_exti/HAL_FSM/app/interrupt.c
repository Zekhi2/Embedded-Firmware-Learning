#include "main.h"
#include "tim.h"

void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}
