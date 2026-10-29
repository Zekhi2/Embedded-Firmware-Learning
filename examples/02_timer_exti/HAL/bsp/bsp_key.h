/* User/bsp/bsp_key.h */
#ifndef BSP_KEY_H
#define BSP_KEY_H

typedef enum {
    KEY_RELEASED = 0,
    KEY_PRESSED  = 1
} key_state_t;

void KEY_Init(void);
key_state_t KEY_Read(void);

/* Day 2 新增：EXTI 中断中只做事件计数，主循环通过接口读取 */
uint32_t KEY_GetEventCount(void);

#endif /* BSP_KEY_H */
