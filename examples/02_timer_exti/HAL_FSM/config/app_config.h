#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "main.h"

/* ---------------- LED 配置 ---------------- */
#define LED_PORT                    LED_GPIO_Port
#define LED_PIN                     LED_Pin

/* 单一有效电平：低电平点亮 */
#define LED_ACTIVE_LEVEL            GPIO_PIN_RESET

/* LED 非阻塞闪烁周期（ms） */
#define LED_BLINK_PERIOD_MS         500U

/* ---------------- KEY 配置 ---------------- */
#define KEY_PORT                    KEY_GPIO_Port
#define KEY_PIN                     KEY_Pin

/* 单一有效电平：按下为低电平 */
#define KEY_PRESSED_LEVEL           GPIO_PIN_RESET

/* --------------- 按键状态机参数配置 ----------------- */
#define KEY_SCAN_PERIOD_MS          10U         /* 按键扫描周期 */
#define KEY_DEBOUNCE_TIME_MS        30U         /* 按键消抖时间 */
#define KEY_LONG_PRESS_TIME_MS      1000U       /* 长按时间 */
#define KEY_REPEAT_TIME_MS          300U        /* 长按触发时间 */
#define KEY_DOUBLE_CLICK_WINDOW_MS  300U        /* 双击等待窗口 */

#endif /* APP_CONFIG_H */
