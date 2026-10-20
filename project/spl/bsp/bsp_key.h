/**
 * @file    bsp_key.h
 * @brief   按键 EXTI 事件接口（SPL 版本，Day 2）
 */

#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>

void bsp_key_init(void);
uint32_t bsp_key_get_event_count(void);
void bsp_key_exti_irq_handler(void);

#endif /* BSP_KEY_H */
