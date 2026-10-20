# Day 2: EXTI 按键 + Timer 系统节拍（feature/periph-systick-exti）

在 Day 1（`feature/drv-gpio`，GPIO LED/按键）基础上，新增：

- EXTI 外部中断检测按键（下降沿触发，中断中只做事件计数）
- TIM3 定时中断产生 1ms 系统节拍
- 基于节拍的**非阻塞** LED 闪烁（500ms）
- 按键事件立即翻转 LED 并重置闪烁节拍

## 目录结构（与 examples/01_gpio_led 对齐）

```text
examples/02_systick_exti/
├── SPL/                    # 标准外设库版本（沿用 Day 1 drv_gpio/gpio_types 分层）
│   ├── app/                # main.c（业务逻辑）、interrupt.c（中断转发）
│   ├── bsp/                # bsp_led、bsp_key（EXTI 事件计数）
│   ├── driver/             # drv_gpio（Day 1 复用）、drv_timer_tick（TIM3 1ms 节拍）
│   ├── common/             # gpio_types.h（电平/上下拉枚举）
│   └── config/             # app_config.h（引脚/时钟/定时器集中配置）
└── HAL/                    # HAL 库版本（同构目录）
    ├── app/
    ├── bsp/
    ├── driver/
    └── config/
```

## 设计约定（沿用 Day 1）

- `main.c` 只调用接口，不直接操作寄存器；
- 共享变量为模块内部 `static volatile`，通过 getter 接口读取；
- 时间变量统一 `uint32_t`，超时判断使用差值写法 `(now - last) >= timeout`，可正确处理溢出回绕；
- SPL 版沿用 Day 1 的 `LED_ON_LEVEL` / `KEY_PRESSED_LEVEL` / `KEY_INTPUT_PULL` 等宏与 `drv_gpio_*` 驱动层，新增 `KEY_EXTI_*`、`SYSTEM_TICK_TIM_*` 两组配置宏；
- BSP 接口沿用 Day 1 命名风格：`LED_Init/LED_On/LED_Off/LED_Toggle`、`KEY_Init/KEY_Read`，Day 2 新增 `KEY_GetEventCount`；
- `interrupt.c` 只做中断入口转发，不写业务逻辑。

## SPL / HAL 差异

| 项目 | SPL 版本 | HAL 版本 |
|---|---|---|
| GPIO 初始化 | 复用 Day 1 `drv_gpio_*` | 模块内直接 HAL GPIO 调用 |
| EXTI 回调 | `KEY_EXTI_IRQHandler` 自行清标志 | `HAL_GPIO_EXTI_Callback` |
| Timer 回调 | `drv_timer_tick_irq_handler` 自行清标志 | `HAL_TIM_PeriodElapsedCallback` |
| 中断入口 | 转发到模块处理函数 | 调用 `HAL_xxx_IRQHandler` |
