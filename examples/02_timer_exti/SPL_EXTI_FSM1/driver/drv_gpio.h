#ifndef DRV_GPIO_H
#define DRV_GPIO_H

#include "stm32f10x.h"
#include <stdint.h>

void drv_gpio_output_init(GPIO_TypeDef *port,
                          uint32_t rcc,
                          uint16_t pin);

void drv_gpio_input_init(GPIO_TypeDef *port,
                         uint32_t rcc,
                         uint16_t pin,
                         uint8_t pull);

void drv_gpio_write(GPIO_TypeDef *port,
                    uint16_t pin,
                    BitAction level);

uint8_t drv_gpio_read(GPIO_TypeDef *port,
                      uint16_t pin);

void drv_gpio_toggle(GPIO_TypeDef *port,
                     uint16_t pin);

#endif  /* DRV_GPIO_H */