/* User/bsp/bsp_key.h */
#ifndef BSP_KEY_H
#define BSP_KEY_H

#include <stdint.h>

typedef enum {
    KEY_RELEASED = 0,
    KEY_PRESSED  = 1
} key_state_t;

void KEY_Init(void);
key_state_t KEY_Read(void);

#endif