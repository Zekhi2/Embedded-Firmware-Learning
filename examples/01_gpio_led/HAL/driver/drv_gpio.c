/* User/driver/drv_gpio.c */
#include "drv_gpio.h"

/* 辅助函数：使能时钟 */
static void drv_gpio_clock_enable(GPIO_TypeDef *port)
{
    if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    /* F103 通常用到 A-E */
}

void drv_gpio_output_init(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    drv_gpio_clock_enable(port);

    GPIO_InitStruct.Pin   = pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_input_init(GPIO_TypeDef *port, uint16_t pin, gpio_pull_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    drv_gpio_clock_enable(port);

    GPIO_InitStruct.Pin   = pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    /* 映射自定义枚举到 HAL 宏 */
    if (pull == GPIO_PULL_UP) {
        GPIO_InitStruct.Pull = GPIO_PULLUP;
    } else if (pull == GPIO_PULL_DOWN) {
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    } else {
        GPIO_InitStruct.Pull = GPIO_NOPULL;
    }

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_write(GPIO_TypeDef *port, uint16_t pin, gpio_level_t level)
{
    HAL_GPIO_WritePin(port, pin, (level == GPIO_LEVEL_HIGH) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

gpio_level_t drv_gpio_read(GPIO_TypeDef *port, uint16_t pin)
{
    return (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
}

void drv_gpio_toggle(GPIO_TypeDef *port, uint16_t pin)
{
    HAL_GPIO_TogglePin(port, pin);
}