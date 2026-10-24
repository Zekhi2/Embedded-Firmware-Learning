#include "bsp_led.h"
#include "drv_gpio.h"
#include "app_config.h"

/* LED软件状态标记 */
static uint8_t s_led_state = 0;

void LED_Init(void)
{
    drv_gpio_output_init(LED_GPIO_PORT,
                         LED_GPIO_CLK,
                         LED_GPIO_PIN);

    LED_Off();
}

void LED_On(void)
{
    s_led_state = 1;

    drv_gpio_write(LED_GPIO_PORT,
                   LED_GPIO_PIN,
                   LED_ON_LEVEL);
}
a
void LED_Off(void)
{
    s_led_state = 0;

    drv_gpio_write(LED_GPIO_PORT,
                   LED_GPIO_PIN,
                   !LED_ON_LEVEL);
}

void LED_Toggle(void)
{
    drv_gpio_toggle(LED_GPIO_PORT, LED_GPIO_PIN);
    s_led_state = !s_led_state;
}