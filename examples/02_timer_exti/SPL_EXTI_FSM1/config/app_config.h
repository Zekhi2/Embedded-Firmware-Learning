#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f10x.h"
#include "gpio_types.h"

/* ---------------- LED 配置 ---------------- */

/* 假设 LED 接在 PC13 */
#define LED_GPIO_PORT        GPIOC
#define LED_GPIO_PIN         GPIO_Pin_13
#define LED_GPIO_CLK         RCC_APB2Periph_GPIOC

/* LED 点亮时对应引脚电平
0：低电平点亮；1：高电平点亮 */
#define LED_ON_LEVEL        Bit_RESET

/* ---------------- KEY 配置 ---------------- */

/* 假设按键接在 PA0 */
#define KEY_GPIO_PORT        GPIOA
#define KEY_GPIO_PIN         GPIO_Pin_0
#define KEY_GPIO_CLK         RCC_APB2Periph_GPIOA

/* 客观软件标准——按键按下时对应引脚电平
PRESSED -- 0：按下为低电平；1：按下为高电平 */
typedef enum
{
    KEY_RELEASED_LEVEL = Bit_SET,
    KEY_PRESSED_LEVEL  = Bit_RESET
} key_level_t;

/* 主动硬件配置——按键输入引脚内部偏置方式
1：内部上拉；2：内部下拉； */
#define KEY_INTPUT_PULL      GPIO_PULL_UP

/* ---------------- KEY EXTI 配置 ---------------- */

#define KEY_EXTI_LINE              EXTI_Line0
#define KEY_EXTI_PORT_SOURCE       GPIO_PortSourceGPIOA
#define KEY_EXTI_PIN_SOURCE        GPIO_PinSource0
#define KEY_EXTI_IRQn              EXTI0_IRQn

/* --------------- 按键参数配置 ----------------- */
#define KEY_SCAN_PERIOD_MS          10U         /* 按键扫描周期 */
#define KEY_DEBOUNCE_TIME_MS        30U         /* 按键消抖时间 */
#define KEY_LONG_PRESS_TIME_MS      1000U       /* 长按时间 */
#define KEY_REPEAT_TIME_MS          300U        /* 长按触发时间 */

/* ---------------- 系统节拍 Timer ---------------- */

#define SYSTEM_TICK_TIM         TIM3
#define SYSTEM_TICK_TIM_CLK     RCC_APB1Periph_TIM3
#define SYSTEM_TICK_TIM_IRQn    TIM3_IRQn

/*
 * 假设 TIM3 输入时钟为 72MHz：
 *
 * 72MHz / 72 = 1MHz
 * 1MHz 计数 1000 次 = 1ms
 */
#define SYSTEM_TICK_TIM_PSC     (72U - 1U)
#define SYSTEM_TICK_TIM_ARR     (1000U - 1U)

#endif  /* APP_CONFIG_H */