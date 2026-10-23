/* User/config/app_config.h */
#ifndef APP_CONFIG_H
#define APP_CONFIG_H

/* 包含 CubeMX 生成的头文件，获取 LED_Pin, KEY_Pin 等定义 */
#include "main.h"

/* ---------------- LED 配置 ---------------- */
/* 物理引脚映射 (来自 CubeMX 生成的 main.h) */
#define LED_PORT        LED_GPIO_Port
#define LED_PIN         LED_Pin

/* 业务逻辑：低电平点亮*/
#define LED_ON_LEVEL    GPIO_PIN_RESET
#define LED_OFF_LEVEL   GPIO_PIN_SET

/* ---------------- KEY 配置 ---------------- */
/* 物理引脚映射 (来自 main.h) */
#define KEY_PORT        KEY_GPIO_Port
#define KEY_PIN         KEY_Pin

/* 业务逻辑：按下为低电平 (根据原理图 PA0 接 GND) */
#define KEY_PRESSED_LEVEL   GPIO_PIN_RESET
#define KEY_RELEASED_LEVEL  GPIO_PIN_SET

/* CubeMX 在 gpio.c 里配置
SPL配置硬件：内部上拉 */
// #define KEY_INPUT_PULL    GPIO_PULLUP

#endif /* APP_CONFIG_H */