#include "app_config.h"
#include "bsp_key.h"
#include "drv_gpio.h"
#include "drv_timer_tick.h"

static key_fsm_t s_key;

/* 读取具体硬件电平 */
static key_level_t key_read_level(void)
{
    uint8_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    return level;
}

void KEY_Init(void)
{
    key_cfg_t cfg;

    /* GPIO 输入初始化 */
    drv_gpio_input_init(KEY0_GPIO_PORT,
                        KEY0_GPIO_CLK,
                        KEY0_GPIO_PIN,
                        KEY0_INPUT_MODE);

    /* 按键参数配置 */
    cfg.debounce_ms    = KEY_DEBOUNCE_TIME_MS;
    cfg.long_press_ms  = KEY_LONG_PRESS_TIME_MS;
    cfg.repeat_ms      = KEY_REPEAT_TIME_MS;

    /* 初始化通用状态机 */
    key_fsm_init(&s_key, &cfg, key_read_level);
}

/* 周期性扫描 */
key_event_t KEY_Process(void)
{
    uint32_t now;

    now = drv_timer_tick_get();

    return key_fsm_scan(&s_key, now);
}