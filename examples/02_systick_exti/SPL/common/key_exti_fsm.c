#include "key_exti_fsm.h"

void key_exti_fsm_init(key_exti_fsm_t *key, const key_cfg_t *cfg, key_read_level_fn read_level)
{
    key->cfg = *cfg;
    key->read_level = read_level;
    key->state = KEY_FSM_STATE_RELEASED;
    key->state_enter_tick = 0;
    key->last_repeat_tick = 0;
    key->long_press_reported = false;
}

key_event_t key_exti_fsm_process(key_exti_fsm_t *key, uint32_t now, BitAction current_level)
{
    key_event_t event = KEY_EVENT_NONE;
    uint32_t delta = now - key->state_enter_tick;

    switch (key->state)
    {
        case KEY_FSM_STATE_RELEASED:
            if(current_level == Bit_RESET) // 检测到低电平（按下）
            {
                key->state = KEY_FSM_STATE_DEBOUNCE_PRESS;
                key->state_enter_tick = now;
            }
            break;

        case KEY_FSM_STATE_DEBOUNCE_PRESS:
            if(delta >= key->cfg.debounce_ms)
            {
                // 消抖时间到，电平依然是按下 → 确认按下
                if(current_level == Bit_RESET)
                {
                    key->state = KEY_FSM_STATE_PRESSED;
                    key->state_enter_tick = now;
                    event = KEY_EVENT_DOWN;
                    key->long_press_reported = false;
                }
                else
                {
                    // 消抖窗口内电平变回高，是抖动，退回释放态
                    key->state = KEY_FSM_STATE_RELEASED;
                }
            }
            break;

        case KEY_FSM_STATE_PRESSED:
            if(current_level == Bit_SET) // 检测到释放边沿
            {
                key->state = KEY_FSM_STATE_DEBOUNCE_RELEASE;
                key->state_enter_tick = now;
            }
            else
            {
                // 仍然保持按下：判断长按、长按连发
                if(!key->long_press_reported && delta >= key->cfg.long_press_ms)
                {
                    event = KEY_EVENT_LONG_START;
                    key->long_press_reported = true;
                    key->last_repeat_tick = now;
                }
                else if(key->long_press_reported && (now - key->last_repeat_tick) >= key->cfg.repeat_ms)
                {
                    event = KEY_EVENT_LONG_REPEAT;
                    key->last_repeat_tick = now;
                }
            }
            break;

        case KEY_FSM_STATE_DEBOUNCE_RELEASE:
            if(delta >= key->cfg.debounce_ms)
            {
                if(current_level == Bit_SET)
                {
                    key->state = KEY_FSM_STATE_RELEASED;
                    event = KEY_EVENT_UP;
                    // 从稳定按下正常释放，产生单击事件
                    if(!key->long_press_reported)
                    {
                        event = KEY_EVENT_CLICK;
                    }
                }
                else
                {
                    // 抖动，又被拉低，回到按下态
                    key->state = KEY_FSM_STATE_PRESSED;
                }
            }
            break;

        default:
            key->state = KEY_FSM_STATE_RELEASED;
            break;
    }
    return event;
}
