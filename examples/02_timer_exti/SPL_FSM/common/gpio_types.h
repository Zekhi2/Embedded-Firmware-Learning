#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

/* SPL没有原生 PULL 枚举 */
typedef enum
{
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN
} gpio_pull_t;

#endif /* GPIO_TYPES_H */