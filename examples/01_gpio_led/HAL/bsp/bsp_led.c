/* User/bsp/bsp_led.c */
#include "bsp_led.h"
#include "drv_gpio.h"
#include "app_config.h"

void LED_Init(void)
{
    /* 
     * 注意：CubeMX 的 MX_GPIO_Init() 已经初始化了引脚。
     * 如果完全依赖 CubeMX，这里可以不写 drv_gpio_output_init。
     * 但为了模块独立性，或者防止 CubeMX 配置丢失，建议在这里再配置一次，
     * 或者至少确保状态一致。
     * 这里我们选择信任 MX_GPIO_Init，只做逻辑封装。
     * 如果你想完全脱离 MX_GPIO_Init，可以解开下面注释：
     */
    // drv_gpio_output_init(LED_PORT, LED_PIN);
    
    LED_Off(); /* 默认熄灭 */
}

void LED_On(void)
{
    /* 根据配置输出电平 */
    drv_gpio_write(LED_PORT, LED_PIN, LED_ON_LEVEL);
}

void LED_Off(void)
{
    /* 输出相反电平 */
    drv_gpio_write(LED_PORT, LED_PIN, (LED_ON_LEVEL == GPIO_LEVEL_HIGH) ? GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH);
}

void LED_Toggle(void)
{
    drv_gpio_toggle(LED_PORT, LED_PIN);
}