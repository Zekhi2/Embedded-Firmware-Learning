/**
 * @file    bsp_led.h
 * @brief   LED 板级控制接口（SPL 版本，Day 2）
 */

#ifndef BSP_LED_H
#define BSP_LED_H

void bsp_led_init(void);
void bsp_led_on(void);
void bsp_led_off(void);
void bsp_led_toggle(void);

#endif /* BSP_LED_H */
