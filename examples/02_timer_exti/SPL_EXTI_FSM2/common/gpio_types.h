#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

/* 通用引脚电平定义 */
typedef enum
{
    GPIO_LEVEL_LOW  = 0U,
    GPIO_LEVEL_HIGH = 1U
} gpio_level_t;

/* 通用输入偏置定义 */
typedef enum
{
    GPIO_PULL_NONE = 0U,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

#endif /* GPIO_TYPES_H */
