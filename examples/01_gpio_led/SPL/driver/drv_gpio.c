#include "drv_gpio.h"
#include "gpio_types.h"

void drv_gpio_output_init(GPIO_TypeDef *port,
                          uint32_t rcc,
                          uint16_t pin)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 打开 GPIO 时钟 */
    RCC_APB2PeriphClockCmd(rcc, ENABLE);

    GPIO_InitStruct.GPIO_Pin   = pin;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_input_init(GPIO_TypeDef *port,
                         uint32_t rcc,
                         uint16_t pin,
                         gpio_pull_t pull)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* 打开 GPIO 时钟 */
    RCC_APB2PeriphClockCmd(rcc, ENABLE);

    GPIO_InitStruct.GPIO_Pin  = pin;
    
    if (pull == GPIO_PULL_UP)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    }
    else if (pull == GPIO_PULL_DOWN)
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    }
    else
    {
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }

    GPIO_Init(port, &GPIO_InitStruct);
}

void drv_gpio_write(GPIO_TypeDef *port,
                    uint16_t pin,
                    gpio_level_t level)
{
    if (level == GPIO_LEVEL_HIGH)
    {
        GPIO_SetBits(port, pin);
    }
    else
    {
        GPIO_ResetBits(port, pin);
    }
}

gpio_level_t drv_gpio_read(GPIO_TypeDef *port,
                           uint16_t pin)
{
    if (GPIO_ReadInputDataBit(port, pin) != Bit_RESET)
    {
        return GPIO_LEVEL_HIGH;
    }
    else
    {
        return GPIO_LEVEL_LOW;
    }
}

void drv_gpio_toggle(GPIO_TypeDef *port,
                     uint16_t pin)
{
    if (GPIO_ReadOutputDataBit(port, pin) != Bit_RESET)
    {
        GPIO_ResetBits(port, pin);
    }
    else
    {
        GPIO_SetBits(port, pin);
    }
}
