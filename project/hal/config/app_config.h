#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f1xx_hal.h"

/*=====================================================
 * LED 配置
 *=====================================================*/
#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_PIN        GPIO_PIN_13

/* 1：低电平点亮，0：高电平点亮 */
#define LED_ACTIVE_LOW      1

/*=====================================================
 * KEY 配置
 *=====================================================*/
#define KEY_GPIO_PORT       GPIOA
#define KEY_GPIO_PIN        GPIO_PIN_0
#define KEY_EXTI_IRQn       EXTI0_IRQn

/*=====================================================
 * 系统节拍 Timer 配置
 *=====================================================*/
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
