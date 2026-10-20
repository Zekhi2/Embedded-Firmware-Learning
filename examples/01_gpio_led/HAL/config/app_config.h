#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f1xx_hal.h"

/* ---------------- LED 配置 ---------------- */

/* 很多 STM32F103C8T6 开发板 LED 接在 PC13，低电平点亮 */
#define LED_GPIO_PORT        GPIOC
#define LED_GPIO_PIN         GPIO_PIN_13

/* 1：低电平点亮；0：高电平点亮 */
#define LED_ACTIVE_LOW       1


/* ---------------- KEY 配置 ---------------- */

/* 假设按键接在 PA0，按下接地 */
#define KEY_GPIO_PORT        GPIOA
#define KEY_GPIO_PIN         GPIO_PIN_0

/* 1：按下为低电平；0：按下为高电平 */
#define KEY_ACTIVE_LOW       1

/* 1：内部上拉；0：内部下拉 */
#define KEY_PULL_UP          1

#endif