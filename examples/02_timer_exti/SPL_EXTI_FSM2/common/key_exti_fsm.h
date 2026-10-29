#ifndef KEY_EXTI_FSM_H
#define KEY_EXTI_FSM_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_DOWN,
    KEY_EVENT_UP,
    KEY_EVENT_CLICK,
    KEY_EVENT_LONG_START,
    KEY_EVENT_LONG_REPEAT
} key_event_t;

typedef enum
{
    KEY_FSM_STATE_RELEASED = 0,
    KEY_FSM_STATE_DEBOUNCE_PRESS,
    KEY_FSM_STATE_PRESSED,
    KEY_FSM_STATE_DEBOUNCE_RELEASE
} key_fsm_state_t;

typedef struct
{
    uint32_t debounce_ms;
    uint32_t long_press_ms;
    uint32_t repeat_ms;
} key_cfg_t;

// 回调：读取按键当前真实电平 BitAction（沿用你SPL原生BitAction）
typedef BitAction (*key_read_level_fn)(void);

// 按键实例
typedef struct
{
    key_cfg_t          cfg;
    key_read_level_fn  read_level;

    key_fsm_state_t    state;
    uint32_t           state_enter_tick;
    uint32_t           last_repeat_tick;
    bool               long_press_reported;
} key_exti_fsm_t;

void key_exti_fsm_init(key_exti_fsm_t *key, const key_cfg_t *cfg, key_read_level_fn read_level);

// 【EXTI版本的驱动函数】
// 入参：now 当前tick，current_level 本次边沿触发后读到的真实电平
key_event_t key_exti_fsm_process(key_exti_fsm_t *key, uint32_t now, BitAction current_level);

#endif
