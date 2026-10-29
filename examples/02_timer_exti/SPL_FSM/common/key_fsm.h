#ifndef KEY_FSM_H
#define KEY_FSM_H

#include <stdint.h>
#include <stdbool.h>

/* 按键逻辑状态 */
typedef enum
{
    KEY_RELEASED = 0,
    KEY_PRESSED
} key_state_t;

/* 按键事件 */
typedef enum
{
    KEY_EVENT_NONE = 0,     /* 空闲状态 */
    KEY_EVENT_SINGLE_CLICK,
    KEY_EVENT_DOUBLE_CLICK,
    KEY_EVENT_LONG_START,   /* 长按事件 */
    KEY_EVENT_LONG_REPEAT   /* 长按重复事件 */
} key_event_t;

/* 按键状态 */
typedef enum
{
    KEY_FSM_STATE_RELEASED = 0,     /* 稳定释放 */
    KEY_FSM_STATE_DEBOUNCE_PRESS,   /* 检测到按下，正在消抖 */
    KEY_FSM_STATE_PRESSED,          /* 稳定按下 */
    KEY_FSM_STATE_DEBOUNCE_RELEASE, /* 检测到释放，正在消抖 */
    KEY_FSM_STATE_WAIT_DOUBLE,
    KEY_FSM_STATE_DOUBLE_PRESSED
} key_fsm_state_t;

/* 按键配置 
支持多个按键；不写死宏 */
typedef struct
{
    uint32_t debounce_ms;       /* 消抖时长 */
    uint32_t long_press_ms;     /* 长按判定阈值 */
    uint32_t repeat_ms;         /* 长按之后，连续重复触发的间隔 */
    uint32_t double_click_ms;
} key_cfg_t;

/* 按键电平读取接口 */
typedef key_level_t (*key_read_level_fn)(void);

/* 按键状态机对象 */
typedef struct
{
    key_cfg_t          cfg;
    key_read_level_fn  read_level;

    key_fsm_state_t    state;
    uint32_t           state_enter_tick;
    uint32_t           last_repeat_tick;
    uint32_t           double_wait_tick;

    bool               long_active;
    bool               double_active;
    uint8_t            click_count;
} key_fsm_t;

/* 接口函数 */
void key_fsm_init(key_fsm_t           *key,
                  const key_cfg_t     *cfg,
                  key_read_level_fn    read_level);

key_event_t key_fsm_scan(key_fsm_t *key, uint32_t now);

#endif /* KEY_FSM_H */