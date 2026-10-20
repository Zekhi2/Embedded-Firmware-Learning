#include "bsp_key.h"
#include "drv_gpio.h"
#include "app_config.h"

void KEY_Init(void)
{
    drv_gpio_input_init(KEY_GPIO_PORT,
                        KEY_GPIO_PIN,
                        KEY_PULL_UP);
}

static uint8_t key_raw_is_pressed(void)
{
    uint8_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    if (KEY_ACTIVE_LOW == 1)
    {
        /* 低电平有效：读到 0 表示按下 */
        return (level == 0) ? 1 : 0;
    }
    else
    {
        /* 高电平有效：读到 1 表示按下 */
        return (level == 1) ? 1 : 0;
    }
}

key_state_t KEY_Read(void)
{
    if (key_raw_is_pressed() == 1)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_RELEASED;
    }
}