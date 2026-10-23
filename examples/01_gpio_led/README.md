## SPL / HAL 差异
### gpio
`stm32f1xx_hal_gpio.h` 可以替代 `common/gpio_types.h`。
> 当一个驱动层需要完全跨平台时，才需要自定义gpio相关内容。对于纯STM32项目，用HAL头文件提供的相关枚举常量、宏即可。

`common/gpio_types.h`的内容：
```C
/* User/common/gpio_types.h */
#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

#include <stdint.h>

/* 对应 HAL 的 GPIO_PIN_RESET / GPIO_PIN_SET */
typedef enum {
    GPIO_LEVEL_LOW  = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

/* 对应 HAL 的 GPIO_NOPULL / GPIO_PULLUP / GPIO_PULLDOWN */
typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

#endif
```

`stm32f1xx_hal_gpio.h`的内容：
```C
/* 电平状态 */
typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;

/* 上下拉模式 */
#define  GPIO_NOPULL        0x00000000U
#define  GPIO_PULLUP        0x00000001U
#define  GPIO_PULLDOWN      0x00000002U

/* 读写函数 */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```