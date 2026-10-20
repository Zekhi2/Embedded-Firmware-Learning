# Day 2: EXTI 按键 + Timer 系统节拍（feature/periph-systick-exti）

在 Day 1（`feature/drv-gpio`，GPIO LED/按键）基础上，新增：

- EXTI 外部中断检测按键（下降沿触发，中断中只做事件计数）
- TIM3 定时中断产生 1ms 系统节拍
- 基于节拍的**非阻塞** LED 闪烁（500ms）
- 按键事件立即翻转 LED 并重置闪烁节拍

## 目录结构（SPL / HAL 双版本）

```text
project/
├── spl/                    # 标准外设库版本
│   ├── app/                # main.c（业务逻辑）、interrupt.c（中断转发）
│   ├── bsp/                # bsp_led、bsp_key（EXTI 事件计数）
│   ├── driver/             # drv_timer_tick（TIM3 1ms 节拍）
│   └── config/             # app_config.h（引脚/时钟/定时器集中配置）
└── hal/                    # HAL 库版本（同构目录）
    ├── app/
    ├── bsp/
    ├── driver/
    └── config/
```

## 设计约定（沿用 Day 1）

- `main.c` 只调用接口，不直接操作寄存器；
- 共享变量为模块内部 `static volatile`，通过 getter 接口读取；
- 时间变量统一 `uint32_t`，超时判断使用差值写法 `(now - last) >= timeout`，可正确处理溢出回绕；
- 宏命名沿用 `LED_GPIO_*` / `KEY_GPIO_*` / `LED_ACTIVE_LOW` 风格，新增 `SYSTEM_TICK_TIM_*` 一组；
- `interrupt.c` 只做中断入口转发，不写业务逻辑。
