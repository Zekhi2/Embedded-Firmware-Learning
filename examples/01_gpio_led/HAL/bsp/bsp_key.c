/* User/bsp/bsp_key.c */
#include "bsp_key.h"
#include "app_config.h"
#include "stm32f1xx_hal.h"

/* MX_GPIO_Init 已配置 KEY_Init。若需独立初始化：
    * drv_gpio_input_init(KEY_PORT, KEY_PIN, KEY_INPUT_PULL);
    */

key_state_t KEY_Read(void)
{
    GPIO_PinState level;

    level = drv_gpio_read(KEY_PORT, KEY_PIN);

    if (level == KEY_PRESSED_LEVEL) {
        return KEY_PRESSED;
    } else {
        return KEY_RELEASED;
    }
}