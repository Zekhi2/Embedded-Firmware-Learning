#include "drv_gpio.h"

static void drv_gpio_clock_enable(GPIO_TypeDef *port)
{
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else if (port == GPIOD)
    {
        __HAL_RCC_GPIOD_CLK_ENABLE();
    }
    else if (port == GPIOE)
    {
        __HAL_RCC_GPIOE_CLK_ENABLE();
    }
}

void drv_gpio_output_init(GPIO_TypeDef *port,
                          uint16_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    drv_gpio_clock_enable(port);

    GPIO_InitStruct.Pin   = pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_input_init(GPIO_TypeDef *port,
                         uint16_t pin,
                         uint8_t pull_up)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    drv_gpio_clock_enable(port);

    GPIO_InitStruct.Pin   = pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = pull_up ? GPIO_PULLUP : GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_write(GPIO_TypeDef *port,
                    uint16_t pin,
                    uint8_t level)
{
    if (level == 1)
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
    }
    else
    {
        HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
    }
}

uint8_t drv_gpio_read(GPIO_TypeDef *port,
                      uint16_t pin)
{
    if (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void drv_gpio_toggle(GPIO_TypeDef *port,
                     uint16_t pin)
{
    HAL_GPIO_TogglePin(port, pin);
}