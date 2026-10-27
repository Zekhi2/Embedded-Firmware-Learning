#include "key_fsm.h"

/*=====================================================
 * 内部工具：计算经过时间
 *=====================================================*/
static uint32_t key_fsm_elapsed(uint32_t now, uint32_t last)
{
    return (uint32_t)(now - last);
}

/*=====================================================
 * 初始化
 *=====================================================*/
void key_fsm_init(key_fsm_t           *key,
                  const key_cfg_t     *cfg,
                  key_read_level_fn    read_level)
{
    if (key == 0 || cfg == 0 || read_level == 0)
    {
        return;
    }

    key->cfg                 = *cfg;
    key->read_level          = read_level;

    key->state               = KEY_FSM_STATE_RELEASED;
    key->state_enter_tick    = 0;
    key->last_repeat_tick    = 0;
    key->long_press_reported = false;
}

/*=====================================================
 * 状态机扫描
 *=====================================================*/
key_event_t key_fsm_scan(key_fsm_t *key, uint32_t now)
{
    key_event_t event = KEY_EVENT_NONE;
    key_level_t level;

    if (key == 0 || key->read_level == 0)
    {
        return KEY_EVENT_NONE;
    }

    level = key->read_level();

    switch (key->state)
    {
        /*---------------------------------------------
         * 状态 1：稳定释放
         *---------------------------------------------*/
        case KEY_FSM_STATE_RELEASED:
        {
            if (level == KEY_LEVEL_PRESSED)
            {
                key->state            = KEY_FSM_STATE_DEBOUNCE_PRESS;
                key->state_enter_tick = now;
            }
        }
        break;

        /*---------------------------------------------
         * 状态 2：按下消抖
         *---------------------------------------------*/
        case KEY_FSM_STATE_DEBOUNCE_PRESS:
        {
            if (level == KEY_LEVEL_RELEASED)
            {
                /*
                 * 消抖期间又释放了，
                 * 说明可能是抖动，不确认按下。
                 */
                key->state = KEY_FSM_STATE_RELEASED;
            }
            else if (key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.debounce_ms)
            {
                /*
                 * 按下状态稳定超过消抖时间，
                 * 确认按键按下。
                 */
                key->state               = KEY_FSM_STATE_PRESSED;
                key->state_enter_tick    = now;
                key->long_press_reported = false;

                event = KEY_EVENT_DOWN;
            }
        }
        break;

        /*---------------------------------------------
         * 状态 3：确认按下
         *---------------------------------------------*/
        case KEY_FSM_STATE_PRESSED:
        {
            if (level == KEY_LEVEL_RELEASED)
            {
                key->state            = KEY_FSM_STATE_DEBOUNCE_RELEASE;
                key->state_enter_tick = now;
            }
            else
            {
                /*
                 * 判断长按。
                 *
                 * 如果 long_press_ms 配置为 0，
                 * 可以认为不启用长按。
                 */
                if (key->cfg.long_press_ms > 0U &&
                    key->long_press_reported == false &&
                    key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.long_press_ms)
                {
                    key->long_press_reported = true;
                    key->last_repeat_tick    = now;

                    event = KEY_EVENT_LONG_START;
                }
                else if (key->long_press_reported == true &&
                         key->cfg.repeat_ms > 0U &&
                         key_fsm_elapsed(now, key->last_repeat_tick) >= key->cfg.repeat_ms)
                {
                    key->last_repeat_tick = now;

                    event = KEY_EVENT_LONG_REPEAT;
                }
            }
        }
        break;

        /*---------------------------------------------
         * 状态 4：释放消抖
         *---------------------------------------------*/
        case KEY_FSM_STATE_DEBOUNCE_RELEASE:
        {
            if (level == KEY_LEVEL_PRESSED)
            {
                /*
                 * 释放消抖期间又检测到按下，
                 * 说明释放不稳定，回到按下状态。
                 */
                key->state = KEY_FSM_STATE_PRESSED;
            }
            else if (key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.debounce_ms)
            {
                key->state = KEY_FSM_STATE_RELEASED;

                event = KEY_EVENT_UP;

                /*
                 * 如果没有触发长按，
                 * 可以额外产生一次短按事件。
                 */
                if (key->long_press_reported == false)
                {
                    event = KEY_EVENT_CLICK;
                }

                key->long_press_reported = false;
            }
        }
        break;

        default:
        {
            key->state = KEY_FSM_STATE_RELEASED;
        }
        break;
    }

    return event;
}