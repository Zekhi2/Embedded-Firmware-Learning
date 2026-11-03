#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>

/* 按键逻辑状态枚举 */
typedef enum
{
    KEY_RELEASED = 0,
    KEY_PRESSED
} key_state_t;

void KEY_Init(void);
key_state_t KEY_Read(void);
uint32_t KEY_GetEventCount(void);

#endif /* BSP_KEY_H */
