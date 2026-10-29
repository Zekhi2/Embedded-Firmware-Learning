#include "key_fsm.h"

/* 内部工具：计算经过时间 */
static uint32_t key_fsm_elapsed(uint32_t now, uint32_t last)
{
    return (uint32_t)(now - last);
}

/* 初始化 */
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
    key->double_wait_tick    = 0;

    key->long_active         = false;
    key->double_active       = false;
    key->click_count         = 0;
}

/* 状态机扫描 */
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
            if (level == KEY_PRESSED)
            {
                key->click_count      = 0;
                key->long_active      = false;
                key->double_active    = false;

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
                 * 如果当前正在等待第二次按下，
                 * 但第二次按下没有通过消抖，
                 * 则回到等待双击状态，
                 * 继续等待双击窗口超时。
                 */
                if (key->click_count == 1U)
                {
                    key->state = KEY_FSM_STATE_WAIT_DOUBLE;
                }
                else
                {
                    key->state = KEY_FSM_STATE_RELEASED;
                }
            }
            else if (key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.debounce_ms)
            {
                if (key->click_count == 1U)
                {
                    key->state               = KEY_FSM_STATE_DOUBLE_PRESSED;
                    key->state_enter_tick    = now;
                    key->double_active       = true;
                    key->click_count         = 0;

                    event = KEY_EVENT_DOUBLE_CLICK;
                }
                else
                {
                    key->state       = KEY_FSM_STATE_PRESSED;
                    key->state_tick  = now;
                    key->long_active = false;
                }
            }
        }
        break;

        /*---------------------------------------------
         * 状态 3：第一次确认按下
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
                    key->long_active == false &&
                    key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.long_press_ms)
                {
                    key->long_active         = true;
                    key->last_repeat_tick    = now;

                    event = KEY_EVENT_LONG_START;
                }
                else if (key->long_active == true &&
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
                if (key->double_active == true)
                {
                    key->state = KEY_FSM_STATE_DOUBLE_PRESSED;
                }
                else
                {
                    key->state = KEY_FSM_STATE_PRESSED;
                }
            }
            else if (key_fsm_elapsed(now, key->state_enter_tick) >= key->cfg.debounce_ms)
            {
                if (key->double_active == true)
                {
                    /*
                     * 第二次按键释放。
                     * 双击已经触发，不再产生单击。
                     */
                    key->state         = KEY_FSM_STATE_RELEASED;
                    key->double_active = false;
                }
                else if (key->long_active == true)
                {
                    /*
                     * 长按释放。
                     * 不产生单击或双击。
                     */
                    key->state       = KEY_FSM_STATE_RELEASED;
                    key->long_active = false;
                }
                else if (key->cfg.double_click_ms == 0U)
                {
                    /*
                     * 没有启用双击。
                     * 短按释放后立即确认单击。
                     */
                    key->state = KEY_FSM_STATE_RELEASED;

                    event = KEY_EVENT_SINGLE_CLICK;
                }
                else
                {
                    /*
                     * 第一次短按释放。
                     * 进入等待双击状态。
                     */
                    key->state            = KEY_FSM_STATE_WAIT_DOUBLE;
                    key->double_wait_tick = now;
                    key->click_count      = 1U;
                }
            }
        }
        break;

        /*---------------------------------------------
         * 状态 5：等待第二次按下
         *---------------------------------------------*/
        case KEY_FSM_STATE_WAIT_DOUBLE:
        {
            if (level == KEY_LEVEL_PRESSED)
            {
                /*
                 * 双击窗口内检测到第二次按下。
                 * 进入按下消抖。
                 *
                 * 注意：
                 * click_count 保持 1，
                 * 用于在消抖完成后识别为第二次按下。
                 */
                key->state            = KEY_FSM_STATE_DEBOUNCE_PRESS;
                key->state_enter_tick = now;
            }
            else if (key_elapsed(now, key->double_wait_tick) >= key->cfg.double_click_ms)
            {
                /*
                 * 双击窗口超时。
                 * 确认单击。
                 */
                key->state       = KEY_FSM_STATE_RELEASED;
                key->click_count = 0;

                event = KEY_EVENT_SINGLE_CLICK;
            }
        }
        break;

        /*---------------------------------------------
         * 状态 6：第二次确认按下
         *---------------------------------------------*/
        case KEY_FSM_STATE_DOUBLE_PRESSED:
        {
            if (level == KEY_LEVEL_RELEASED)
            {
                key->state            = KEY_FSM_STATE_DEBOUNCE_RELEASE;
                key->state_enter_tick = now;
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