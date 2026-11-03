#include "app_config.h"
#include "bsp_key.h"
#include "drv_timer_tick.h"

static key_fsm_t s_key;

key_state_t KEY_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }

    return KEY_RELEASED;
}

void KEY_Init(void)
{
    key_cfg_t cfg;

    cfg.debounce_ms     = KEY_DEBOUNCE_TIME_MS;
    cfg.long_press_ms   = KEY_LONG_PRESS_TIME_MS;
    cfg.repeat_ms       = KEY_REPEAT_TIME_MS;
    cfg.double_click_ms = KEY_DOUBLE_CLICK_WINDOW_MS;

    key_fsm_init(&s_key, &cfg, KEY_Read);
}

key_event_t KEY_Process(void)
{
    uint32_t now = drv_timer_tick_get();

    return key_fsm_scan(&s_key, now);
}
