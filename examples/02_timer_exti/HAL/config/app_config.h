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

/* 客观软件标准——按键按下时对应引脚电平 */
#define KEY_PRESSED_LEVEL   GPIO_PIN_RESET
#define KEY_RELEASED_LEVEL  GPIO_PIN_SET

/* ---------------- KEY EXTI 配置（Day 2 新增） ---------------- */
/* PA0 -> EXTI Line0，下降沿触发，中断向量 EXTI0_IRQn */
#define KEY_EXTI_IRQn       EXTI0_IRQn

/* ---------------- 系统节拍 Timer 配置（Day 2 新增） ---------------- */
#define SYSTEM_TICK_TIM_INSTANCE    TIM3
#define SYSTEM_TICK_TIM_IRQn        TIM3_IRQn

/*
 * 假设 TIM3 输入时钟为 72MHz：
 *
 * 72MHz / 72 = 1MHz
 * 1MHz 计数 1000 次 = 1ms
 */
#define SYSTEM_TICK_TIM_PSC         (72U - 1U)
#define SYSTEM_TICK_TIM_ARR         (1000U - 1U)

#endif /* APP_CONFIG_H */
