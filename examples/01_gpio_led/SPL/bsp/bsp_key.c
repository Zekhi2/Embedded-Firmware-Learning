#include "bsp_key.h"
#include "drv_gpio.h"
#include "app_config.h"

void KEY_Init(void)
{
    drv_gpio_input_init(KEY_GPIO_PORT,
                        KEY_GPIO_CLK,
                        KEY_GPIO_PIN,
                        KEY_INPUT_PULL);
}

key_state_t KEY_Read(void)
{
    uint8_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    if (level == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }
    else
    {
        return KEY_RELEASED;
    }
}