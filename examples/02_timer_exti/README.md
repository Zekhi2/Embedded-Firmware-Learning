# Day 2: EXTI 外部中断 + Timer 系统节拍与按键状态机演进

> 硬件平台：STM32F103C8T6 最小系统板（Blue Pill）  
> 板载资源：LED → PC13（低电平点亮）；KEY → PA0（按下接地，内部上拉）  
> 继承基线：基于 Day 1（`feature/drv-gpio`）五层分层架构演进，新增时基驱动与异步中断处理

---

## 一、 今日新增任务与技术目标

在 Day 1 纯轮询阻塞控制的基础上，解决嵌入式系统核心的**异步输入事件捕获**与**非阻塞时间片轮询**：

1. **时钟与硬件时基（Time-Base）**：利用通用定时器 TIM3 产生精准 **1ms 硬件定时中断**，建立系统全局时间标尺；
2. **非阻塞并发调度**：LED 彻底告别 `delay()` 阻塞死等，改为基于节拍时间差的 **500ms 非阻塞闪烁**；
3. **输入异步捕获与事件响应**：PA0 配置为 EXTI 下降沿中断，在中断中实现**消抖锁定与事件计数**，按键按下立即翻转 LED 并重置闪烁节拍；
4. **按键检测架构的演进分化**：
   - `SPL` / `HAL`：硬件 EXTI 中断消抖锁定与事件计数范式；
   - `SPL_FSM` / `HAL_FSM`：纯定时扫描型有限状态机（单击、双击、长按、连发）；
   - `SPL_EXTI_FSM1`：EXTI 中断置 Flag + 状态机动态休眠/激活；
   - `SPL_EXTI_FSM2`：EXTI 中断 Delta 累加 + 状态机保底推演。

---

## 二、 工程目录架构（SPL / HAL 对称性）

Day 2 沿用并深化了 Day 1 确立的 `app -> bsp -> driver -> 官方库` 单向依赖体系：

```text
examples/02_timer_exti/
├── SPL/                    # 标准外设库版本：EXTI 中断消抖计数 + TIM3 1ms 节拍
│   ├── app/                # main.c（局部快照调度）、interrupt.c（中断向量转发）
│   ├── bsp/                # bsp_led（单宏推导）、bsp_key（EXTI 回调与消抖锁定计数）
│   ├── driver/             # drv_gpio（强类型读写）、drv_timer_tick（TIM3 1ms 时基驱动）
│   ├── common/             # gpio_types.h（电平/上下拉强类型枚举）
│   └── config/             # app_config.h（引脚映射、消抖阈值、有效电平）
│
├── HAL/                    # HAL 库版本（与 SPL 同构）
│   ├── exti_timer.ioc      # STM32CubeMX 图形配置元数据
│   ├── app/                # main.c（SystemClock_Config 72MHz + 快照调度）、interrupt.c
│   ├── bsp/                # bsp_led、bsp_key（HAL_GPIO_EXTI_Callback）
│   ├── driver/             # drv_timer_tick（包装 MX_TIM3_Init 与 HAL_TIM_Base_Start_IT）
│   ├── config/             # app_config.h（映射 main.h 引脚宏）
│   └── Core/               # CubeMX 生成的硬件外设初始化（gpio.c/h, tim.c/h, main.h）
│
└── HAL_FSM/ / SPL_FSM/     # 按键有限状态机版本（新增 common/key_fsm 纯逻辑组件）
```

---

## 三、 核心技术点对比与深度剖析（SPL vs HAL）

### 3.1 硬件时基与定时器驱动（TIM3 1ms Tick）

* **时钟计算**：
  SYSCLK = 72MHz，TIM3 挂载于 APB1（36MHz），根据 F1 时钟树特性，当 APB1 预分频器不为 1 时，定时器倍频器自动开启（$\times 2$），故 TIM3 输入时钟频率为 **72MHz**。
  $$\text{溢出周期} = \frac{(\text{PSC} + 1) \times (\text{ARR} + 1)}{\text{TIM\_CLK}} = \frac{72 \times 1000}{72\,\text{MHz}} = 1\,\text{ms}$$
* **SPL 写法**：在 `driver/drv_timer_tick.c` 中顺序调用 `TIM_TimeBaseInit`、`TIM_ITConfig`、`NVIC_Init`、`TIM_Cmd`。
* **HAL 写法与 CubeMX 暗坑**：
  CubeMX 生成的 `tim.c` 中只执行了 `HAL_TIM_Base_Init(&htim3)`，**并未启动定时器计数器与更新中断**。
  驱动层 `drv_timer_tick.c` 必须补齐关键调用：
  ```c
  void drv_timer_tick_init(void)
  {
      MX_TIM3_Init();
      HAL_TIM_Base_Start_IT(&htim3); /* 开启时基更新中断 */
  }
  ```

### 3.2 外部中断 EXTI 映射与清除机制

* **AFIO 映射机制**：
  PA0 映射到 EXTI0，必须使能 AFIO 外设时钟。SPL 需显式调用 `RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE)` 与 `GPIO_EXTILineConfig()`；HAL 库的 `HAL_GPIO_Init()` 检测到 Pin 属性包含 `IT` 时，内部自动完成 AFIO 时钟使能与端口路由寄存器配置。
* **中断标志位清除（Pending Bit Clear）**：
  - SPL 必须在 `KEY_EXTI_IRQHandler()` 入口显式调用 `EXTI_ClearITPendingBit(KEY_EXTI_LINE)`；
  - HAL 由 `stm32f1xx_it.c` 调用 `HAL_GPIO_EXTI_IRQHandler()`，该函数**在执行用户回调前第一时间硬件清标志位**，规避了初学者因漏清标志而陷入中断死锁的缺陷。

### 3.3 中断消抖锁定与事件计数（Debounce Lockout）

按键按下时机械触点存在 5~20ms 弹性抖动。在 EXTI 中断下，机械抖动会触发多次中断风暴。系统采用**时间窗口锁定机制**：
```c
/* 位于 HAL/bsp/bsp_key.c 的中断回调 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == KEY_PIN)
    {
        uint32_t now = drv_timer_tick_get();
        if ((uint32_t)(now - s_key_last_press_tick) >= KEY_DEBOUNCE_TIME_MS)
        {
            s_key_last_press_tick = now;
            s_key_event_count++;
        }
    }
}
```

---

## 四、 嵌入式工业级规范与标准

本项目代码参考汽车与高可靠嵌入式领域两大核心标准：**MISRA C:2012** 与 **Barr Group Embedded C Coding Standard**。

### 4.1 强类型定义与显式位宽
* **标准规则**：
  - **MISRA C:2012 Dir 4.6**：不得使用基础数值类型（`int`, `char`, `short`），必须使用带有大小与符号明确的 typedef。
  - **Barr Group Rule 1.3.a**：整型定义必须具备显式尺寸与符号性。
* **代码落地**：
  统一引入 `<stdint.h>` 与 `<stdbool.h>`，使用 `uint32_t`, `uint8_t`, `bool`；枚举变量显式限定取值范围（如 `gpio_level_t`）。

### 4.2 单一有效电平与隐式类型转换规避
* **标准规则**：
  - **MISRA C:2012 Rule 10.1 / 10.3**：禁止隐式类型转换，操作数必须具备相同的基本类型范畴。
  - **Barr Group Rule 5.2.b**：尽量避免显式强制转换（Casting），禁止为了掩盖设计缺陷进行不必要的强转。
* **代码落地**：
  1. `app_config.h` 仅暴露业务有效电平（`LED_ACTIVE_LEVEL` / `KEY_PRESSED_LEVEL`）；
  2. 熄灭电平由 BSP 在内部根据逻辑互补安全推导，杜绝逻辑非（`!`）导致的值溢出风险：
     ```c
     #define LED_INACTIVE_LEVEL  ((GPIO_PinState)(1U - (uint32_t)LED_ACTIVE_LEVEL))
     ```
  3. `drv_gpio_read` 返回强类型 `gpio_level_t`，消灭了 `(gpio_level_t)raw_level` 强制转换。

### 4.3 中断并发安全与易失性修饰（Concurrency & Volatile）
* **标准规则**：
  - **Barr Group Rule 1.8.a / 1.8.c**：在 ISR 与主循环/任务间共享的变量必须强制声明为 `volatile`，防止编译器将其寄存器化导致不可见。
  - **Barr Group Rule 7.1.a**：跨上下文访问多字节共享变量时必须防范数据撕裂与竞态条件。
* **代码落地**：
  ```c
  static volatile uint32_t s_system_tick_ms = 0;
  static volatile uint32_t s_key_event_count = 0;
  ```
  主循环在读取共享变量时，**一律先获取局部快照（Snapshot）**：
  ```c
  uint32_t now = drv_timer_tick_get();
  uint32_t key_event_count_snapshot = KEY_GetEventCount();
  ```
  避免一次调度判断中多次读取导致时间戳或状态不一致。

### 4.4 32 位无符号整型回绕安全计算（Unsigned Rollover Safety）
* **标准规则**：
  - **C99 标准 6.2.5 §9**：无符号数算术运算绝不发生溢出（定义为模 $2^n$ 回绕，属于良定义行为）。
  - **Barr Group Rule 5.1.a**：时间戳比对必须使用无符号减法，严禁加法预估超时。
* **代码落地**：
  - **合规写法（安全回绕）**：
    ```c
    if ((uint32_t)(now - led_last_tick) >= LED_BLINK_PERIOD_MS)
    ```
    当 `now` 在约 49.7 天发生回绕（从 `0xFFFFFFFF` 变为 `0x00000000`）时，`0x00000002 - 0xFFFFFFFD` 依模算术规则依然正确计算为 `5`，系统永远不会卡死。
  - **违规写法（严禁使用）**：
    `if (now >= led_last_tick + LED_BLINK_PERIOD_MS)`。一旦加法溢出回绕，条件判定将发生长达 49.7 天的死锁。

### 4.5 零死代码原则（Dead Code Elimination）
* **标准规则**：
  - **MISRA C:2012 Rule 2.2**：工程中不得存在无执行结果的死代码（Dead Code）。
  - **MISRA C:2012 Rule 2.5**：工程中不得声明未使用的宏（Unused Macro）。
* **代码落地**：
  按键只需与 `KEY_PRESSED_LEVEL` 判定是否命中按下事件，彻底移除未被引用的 `KEY_RELEASED_LEVEL` 死代码宏。

### 4.6 极简 ISR 原则（Lean ISR）
* **标准规则**：
  - **Barr Group Rule 8.1.a**：ISR 必须短小精悍，严禁包含延时、复杂状态推演或阻塞调用。
* **代码落地**：
  `TIM3_IRQHandler` 仅做节拍累加；`EXTI0_IRQHandler` 仅做窗口校验与事件计数递增，业务动作（LED 翻转与状态转移）全部交由主循环解耦执行。
