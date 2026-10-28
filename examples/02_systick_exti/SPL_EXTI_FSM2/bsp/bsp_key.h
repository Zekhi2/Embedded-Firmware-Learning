#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>
#include "app_config.h"

void KEY_Init(void);
key_level_t KEY_Read(void);

/* EXTI 中断中只做事件计数，主循环通过接口读取 */
uint32_t KEY_GetEventCount(void);
void KEY_EXTI_IRQHandler(void);

#endif /* BSP_KEY_H */