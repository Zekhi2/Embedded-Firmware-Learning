#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>
#include "key_fsm.h"

void KEY_Init(void);
key_event_t KEY_Process(void);
key_state_t KEY_Read(void);

#endif /* BSP_KEY_H */
