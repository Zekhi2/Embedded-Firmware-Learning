#ifndef DRV_TIMER_TICK_H
#define DRV_TIMER_TICK_H

#include <stdint.h>

void drv_timer_tick_init(void);
uint32_t drv_timer_tick_get(void);
void drv_timer_tick_irq_handler(void);

#endif /* DRV_TIMER_TICK_H */