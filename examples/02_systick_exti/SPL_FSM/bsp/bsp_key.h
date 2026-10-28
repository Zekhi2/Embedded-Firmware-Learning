#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>
#include "key_fsm.h"

void KEY_Init(void);
key_event_t KEY_Process(void);
// key_level_t KEY_Read(void);
// uint32_t KEY_GetEventCount(void);
// void KEY_EXTI_IRQHandler(void);

#endif /* BSP_KEY_H */