#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "app_config.h"
#include "key_fsm.h"

void KEY_Init(void);
key_state_t KEY_Read(void);

void KEY_NotifyEdge(void);
bool KEY_TakeEdgeFlag(void);
key_event_t KEY_Process(void);

void KEY_EXTI_IRQHandler(void);

#endif /* BSP_KEY_H */