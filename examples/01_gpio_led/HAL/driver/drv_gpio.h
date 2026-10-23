/* User/driver/drv_gpio.h */
#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include "stm32f1xx_hal.h"

void drv_gpio_output_init(GPIO_TypeDef *port, uint16_t pin);
void drv_gpio_input_init(GPIO_TypeDef *port, uint16_t pin, GPIO_PullType pull);

void drv_gpio_write(GPIO_TypeDef *port, uint16_t pin, GPIO_PinState state);
GPIO_PinState drv_gpio_read(GPIO_TypeDef *port, uint16_t pin);
void drv_gpio_toggle(GPIO_TypeDef *port, uint16_t pin);

#endif