#include "bsp_key.h"
#include "drv_gpio.h"
#include "drv_timer_tick.h"
#include "app_config.h"
#include "stm32f10x.h"

/* 按键状态机对象 */
static key_fsm_t s_key;
/* EXTI 边沿标志 */
static volatile bool s_key_edge_flag = false;

/* 把按键对应的 GPIO 配置成外部中断输入，
并允许 CPU 响应按键中断。
包含：GPIO 输入初始化、EXTI 初始化、NVIC 初始化、状态机初始化 */
void KEY_Init(void)
{
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    key_cfg_t cfg;

    /* GPIO 输入初始化（内部上拉） */
    drv_gpio_input_init(KEY_GPIO_PORT,
                        KEY_GPIO_CLK,
                        KEY_GPIO_PIN,
                        KEY_INPUT_PULL);

    /* 使能 AFIO 时钟，EXTI 线映射依赖 AFIO */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    /* 将 PA0 映射到 EXTI Line0 */
    GPIO_EXTILineConfig(KEY_EXTI_PORT_SOURCE, KEY_EXTI_PIN_SOURCE);

    /* 配置 EXTI */
    EXTI_InitStruct.EXTI_Line    = KEY_EXTI_LINE;
    EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = KEY_EXTI_TRIGGER;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    /* 清除可能已经存在的 EXTI 挂起标志 */
    EXTI_ClearITPendingBit(KEY_EXTI_LINE);

    /* 配置 NVIC */
    NVIC_InitStruct.NVIC_IRQChannel                   = KEY_EXTI_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 初始化通用状态机 */
    cfg.debounce_ms     = KEY_DEBOUNCE_TIME_MS;
    cfg.long_press_ms   = KEY_LONG_PRESS_TIME_MS;
    cfg.repeat_ms       = KEY_REPEAT_TIME_MS;
    cfg.double_click_ms = KEY_DOUBLE_CLICK_WINDOW_MS;
    
    key_fsm_init(&s_key, &cfg, KEY_Read);
}

/* 读取按键状态，并转换为通用按键逻辑状态 */
key_state_t KEY_Read(void)
{
    gpio_level_t level;

    level = drv_gpio_read(KEY_GPIO_PORT, KEY_GPIO_PIN);

    if (level == KEY_PRESSED_LEVEL)
    {
        return KEY_PRESSED;
    }
    
    return KEY_RELEASED;
}

/* 由 EXTI 中断入口调用，只标记“有边沿发生” */
void KEY_NotifyEdge(void)
{
    s_key_edge_flag = true;
}

/* 主循环调用，取出边沿标志 */
bool KEY_TakeEdgeFlag(void)
{
    bool flag;

    flag = s_key_edge_flag;
    s_key_edge_flag = false;

    return flag;
}

/* 主循环周期调用 */
key_event_t KEY_Process(void)
{
    uint32_t now;

    now = drv_timer_tick_get();

    return key_fsm_scan(&s_key, now);
}

void KEY_EXTI_IRQHandler(void)
{
    if (EXTI_GetITStatus(KEY_EXTI_LINE) != RESET)
    {
        EXTI_ClearITPendingBit(KEY_EXTI_LINE);

        KEY_NotifyEdge();
    }
}
