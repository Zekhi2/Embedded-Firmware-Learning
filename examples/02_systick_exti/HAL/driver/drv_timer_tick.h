#ifndef DRV_TIMER_TICK_H
#define DRV_TIMER_TICK_H

#include "app_config.h"
#include <stdint.h>

extern TIM_HandleTypeDef g_timer_tick_htim;

void drv_timer_tick_init(void);
uint32_t drv_timer_tick_get(void);

#endif /* DRV_TIMER_TICK_H */
