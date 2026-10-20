/**
 * @file    app_config.h
 * @brief   集中配置：引脚 / 时钟 / 定时器参数（SPL 版本）
 * @note    承接 Day 1（feature/drv-gpio）的宏命名风格：
 *          LED_GPIO_CLK / LED_GPIO_PORT / LED_GPIO_PIN / LED_ACTIVE_LOW，
 *          KEY_GPIO_* / KEY_EXTI_*；Day 2 新增 SYSTEM_TICK_TIM_* 一组宏。
 */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f10x.h"

/*=====================================================
 * LED 配置（Day 1 沿用）
 *=====================================================*/
#define LED_GPIO_CLK        RCC_APB2Periph_GPIOC
#define LED_GPIO_PORT       GPIOC
#define LED_GPIO_PIN        GPIO_Pin_13

/* 1：低电平点亮，0：高电平点亮 */
#define LED_ACTIVE_LOW      1

/*=====================================================
 * KEY 配置（Day 2：由轮询扩展为 EXTI 外部中断）
 *=====================================================*/
#define KEY_GPIO_CLK        RCC_APB2Periph_GPIOA
#define KEY_GPIO_PORT       GPIOA
#define KEY_GPIO_PIN        GPIO_Pin_0

#define KEY_EXTI_LINE           EXTI_Line0
#define KEY_EXTI_PORT_SOURCE    GPIO_PortSourceGPIOA
#define KEY_EXTI_PIN_SOURCE     GPIO_PinSource0
#define KEY_EXTI_IRQn           EXTI0_IRQn

/*=====================================================
 * 系统节拍 Timer 配置（Day 2 新增）
 *=====================================================*/
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

#endif /* APP_CONFIG_H */
