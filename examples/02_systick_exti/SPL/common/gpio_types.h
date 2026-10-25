#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

typedef enum
{
    GPIO_LEVEL_LOW  = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

#endif /* GPIO_TYPES_H */