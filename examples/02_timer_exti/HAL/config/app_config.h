#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "main.h"

/* ---------------- LED 配置 ---------------- */
#define LED_PORT                LED_GPIO_Port
#define LED_PIN                 LED_Pin

/* 单一有效电平：低电平点亮 */
#define LED_ACTIVE_LEVEL        GPIO_PIN_RESET

/* LED 非阻塞闪烁周期（ms） */
#define LED_BLINK_PERIOD_MS     500U

/* ---------------- KEY 配置 ---------------- */
#define KEY_PORT                KEY_GPIO_Port
#define KEY_PIN                 KEY_Pin

/* 单一有效电平：按下为低电平 */
#define KEY_PRESSED_LEVEL       GPIO_PIN_RESET

/* ---------------- KEY EXTI 配置 ---------------- */
#define KEY_EXTI_IRQn           EXTI0_IRQn

/* 中断消抖锁定时间（ms） */
#define KEY_DEBOUNCE_TIME_MS    50U

#endif /* APP_CONFIG_H */
