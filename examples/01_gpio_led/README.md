# Day 1: GPIO 控制与分层工程框架搭建（feature/drv-gpio）

> 硬件平台：STM32F103C8T6 最小系统板（Blue Pill）  
> 板载资源：LED → PC13（低电平点亮）；KEY → PA0（按下接地，内部上拉）  
> 工具链：Keil MDK；HAL 版基线由 STM32CubeMX 生成

---

## 一、今日任务

### 1. 项目硬性内容

- [x] 手工搭建 SPL 版工程模板（CMSIS + StdPeriph_Driver + 启动文件），编译下载跑通
- [x] 用 STM32CubeMX 生成 HAL 版工程基线（Core / Drivers）
- [x] 实验 1：点亮 LED（PC13 推挽输出，低电平点亮）
- [x] 实验 2：按键控制 LED（按下亮、松开灭）

### 2. 规范辅助内容

- [x] 确立用户代码五层框架：`app / bsp / driver / common / config`
- [x] 语义化命名：`LED_ON_LEVEL` / `KEY_PRESSED_LEVEL` / `KEY_INPUT_PULL`
  （取代 `LED_ACTIVE_LOW` / `KEY_ACTIVE_LOW` / `KEY_PULL_UP`）
- [x] 确立 include 单向规则：`app → bsp → driver → 官方库`；
  `config / common` 横向被包含，不在纵向调用链上
- [x] Git 在`develop`上建立`feature/drv-gpio`分支并在完成工程后合并到`develop`

---

## 二、两版目录架构（仅代码）

### 2.1 SPL 版

```text
SPL/
├── app/
│   └── main.c                   # 应用层：初始化 + 主循环（按键控制 LED）
├── bsp/
│   ├── bsp_led.c / bsp_led.h    # 板级：LED_On/Off/Toggle，封装低电平点亮语义
│   └── bsp_key.c / bsp_key.h    # 板级：KEY_Read，封装按下电平语义
├── driver/
│   └── drv_gpio.c / drv_gpio.h  # 芯片级：GPIO 初始化/读写/翻转（调 SPL 库）
├── common/
│   └── gpio_types.h             # 通用类型：gpio_level_t / gpio_pull_t
├── config/
│   └── app_config.h             # 板级配置：引脚、时钟宏、有效电平、偏置方式
├── libraries/
│   ├── CMSIS/                   # 官方：core_cm3.h + stm32f10x.h 寄存器定义
│   └── STM32F10x_StdPeriph_Driver/   # 官方：SPL 标准外设库（stm32f10x_gpio.c 等）
└── startup/
    └── startup_stm32f10x_md.s   # 官方：启动文件（向量表 / 堆栈 / Reset_Handler）
```

### 2.2 HAL 版

```text
HAL/
├── bsp/
│   ├── bsp_led.c / bsp_led.h    # 板级：直调 HAL_GPIO_WritePin / TogglePin
│   └── bsp_key.c / bsp_key.h    # 板级：直调 HAL_GPIO_ReadPin
├── config/
│   └── app_config.h             # 板级配置：引用 main.h 引脚宏 + 语义宏（HAL 原生类型）
├── Core/                        # CubeMX 生成区，只允许改 USER CODE 段
│   ├── Inc/
│   │   ├── main.h               # 生成：LED_Pin/KEY_Pin 等引脚宏
│   │   ├── gpio.h               # 生成：MX_GPIO_Init 原型
│   │   ├── stm32f1xx_hal_conf.h # 生成：HAL 模块开关、HSE_VALUE 等宏
│   │   └── stm32f1xx_it.h       # 生成：中断服务函数原型
│   └── Src/
│       ├── main.c               # 生成：main 入口 / SystemClock_Config / Error_Handler
│       ├── gpio.c               # 生成：MX_GPIO_Init（GPIO 初始化唯一入口）
│       ├── stm32f1xx_hal_msp.c  # 生成：HAL_MspInit（全局底层初始化）
│       ├── stm32f1xx_it.c       # 生成：SysTick_Handler 等系统异常入口
│       └── system_stm32f1xx.c   # 官方：SystemInit / SystemCoreClock
├── Drivers/                     # ST 官方库区，只读不改
│   ├── CMSIS/                   # core_cm3.h + stm32f103xb.h 寄存器定义
│   └── STM32F1xx_HAL_Driver/    # HAL 实现（stm32f1xx_hal_gpio.c 等）
└── （启动文件 startup_stm32f103xb.s 位于 IDE 工程目录，参与编译，此处不列）
```

### 2.3 结构差异说明

- HAL 版**没有 `driver/` 与 `common/`**：
  - GPIO 的 HAL API 已是终态语义（无句柄、无状态、一次调用完成），封装零收益，`driver` 层对 GPIO 退役；
  - `gpio_types.h` 被 `stm32f1xx_hal_gpio.h` 原生类型取代（见 3.1）。
- 两版调用关系一致：

```text
纵向：app/main.c → bsp → (driver →) 官方库 → 寄存器 → 硬件
横向：config / common 被各层 #include，不参与调用链
```

---

## 三、SPL / HAL 写法差异

### 3.1 类型层：`stm32f1xx_hal_gpio.h` 可以替代 `common/gpio_types.h`

当一个驱动层需要完全跨平台时，才需要自定义 gpio 相关内容。
对于纯 STM32 项目，用 HAL 头文件提供的枚举常量、宏即可。

`common/gpio_types.h` 的内容（SPL 版自用）：

```c
/* User/common/gpio_types.h */
#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

#include <stdint.h>

/* 对应 HAL 的 GPIO_PIN_RESET / GPIO_PIN_SET */
typedef enum {
    GPIO_LEVEL_LOW  = 0,
    GPIO_LEVEL_HIGH = 1
} gpio_level_t;

/* 对应 HAL 的 GPIO_NOPULL / GPIO_PULLUP / GPIO_PULLDOWN */
typedef enum {
    GPIO_PULL_NONE = 0,
    GPIO_PULL_UP   = 1,
    GPIO_PULL_DOWN = 2
} gpio_pull_t;

#endif
```

`stm32f1xx_hal_gpio.h` 的内容（HAL 原生提供）：

```c
/* 电平状态 */
typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;

/* 上下拉模式 */
#define  GPIO_NOPULL        0x00000000U
#define  GPIO_PULLUP        0x00000001U
#define  GPIO_PULLDOWN      0x00000002U

/* 读写函数 */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
```

结论：HAL 的 `GPIO_PinState` 是强类型枚举，语义已足够清晰，
且 `HAL_GPIO_WritePin` 的形参强制要求该类型——自定义类型反而需要额外映射。
因此 HAL 版删除 `gpio_types.h`，配置宏直接取 HAL 常量。

配置层常量映射表：

| 语义 | SPL 常量（自定义） | HAL 常量（原生） |
|---|---|---|
| 点亮电平（低有效） | `GPIO_LEVEL_LOW` | `GPIO_PIN_RESET` |
| 按下电平（低有效） | `GPIO_LEVEL_LOW` | `GPIO_PIN_RESET` |
| 内部上拉 | `GPIO_PULL_UP` | `GPIO_PULLUP` |
| 内部下拉 | `GPIO_PULL_DOWN` | `GPIO_PULLDOWN` |
| 无偏置 | `GPIO_PULL_NONE` | `GPIO_NOPULL` |

### 3.2 初始化层：深入 `gpio.c` 的 `MX_GPIO_Init()`（CubeMX 生成）

生成代码逐句对照 SPL 写法：

```c
__HAL_RCC_GPIOC_CLK_ENABLE();      /* ↔ SPL: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE) */
__HAL_RCC_GPIOD_CLK_ENABLE();      /* HSE 晶振接 PD0/PD1(OSC_IN/OUT)，CubeMX 顺带使能 GPIOD 时钟 */
__HAL_RCC_GPIOA_CLK_ENABLE();      /* ↔ SPL: RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE) */

/* 先写初始电平，再配置模式：避免配置瞬间出现毛刺电平 */
HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);   /* ↔ SPL: GPIO_SetBits()，高电平 = 灭 */

GPIO_InitStruct.Pin   = LED_Pin;
GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;  /* ↔ SPL: GPIO_Mode_Out_PP 推挽输出 */
GPIO_InitStruct.Pull  = GPIO_NOPULL;          /* SPL 无独立字段，见下注 */
GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;  /* ↔ SPL: GPIO_Speed_50MHz */
HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);           /* ↔ SPL: GPIO_Init() */

GPIO_InitStruct.Pin   = KEY_Pin;
GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;      /* 方向：输入 */
GPIO_InitStruct.Pull  = GPIO_PULLUP;          /* 偏置：上拉（两字段解耦） */
HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);
```

关键差异——**方向与偏置解耦**：

- HAL：`Mode`（方向/复用）与 `Pull`（偏置）是两个独立字段，可自由组合。
- SPL：两者耦合在一个 `GPIO_Mode` 枚举里
  （`GPIO_Mode_Out_PP` / `GPIO_Mode_IPU` / `GPIO_Mode_IPD` / `GPIO_Mode_IN_FLOATING` …），
  改偏置必须换模式常量。
- 这正是 SPL 版需要自定义 `gpio_pull_t` 表达"上拉/下拉/无"三态，
  而 HAL 版可直接用 `GPIO_PULLUP` 宏的原因。

初始化责任差异：

- SPL：初始化代码由用户写在 `driver/drv_gpio.c`（无代码生成工具）。
- HAL：初始化全部由 `MX_GPIO_Init()` 完成，`bsp` 层只做业务逻辑与运行时操作，
  `LED_Init()/KEY_Init()` 中不再重复配置引脚。

### 3.3 系统层：深入 `main.c`（CubeMX 生成）

#### `HAL_Init()`

一条调用完成 SPL 版需要手工分散完成的四件事：

| HAL_Init 内部动作 | SPL 版等价写法 |
|---|---|
| 配置 NVIC 优先级分组（GROUP_4） | `NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4)` |
| 配置 SysTick 1ms 中断（HAL_InitTick） | `SysTick_Config(SystemCoreClock / 1000)` + 自维护 tick 变量 |
| 使能 Flash 预取缓冲 | `FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable)` |
| 回调 `HAL_MspInit()`（见 3.4） | 无对应机制，时钟等散写在各 init 中 |

#### `SystemClock_Config()`

```c
RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
RCC_OscInitStruct.HSEState       = RCC_HSE_ON;          /* 外部 8MHz 晶振 */
RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;        /* 8MHz × 9 = 72MHz SYSCLK */
HAL_RCC_OscConfig(&RCC_OscInitStruct);                  /* 振荡器 + PLL 配置 */

RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;     /* HCLK  = 72MHz */
RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;       /* PCLK1 = 36MHz（APB1 上限） */
RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;       /* PCLK2 = 72MHz */
HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);/* 72MHz 需 2 个 Flash 等待周期 */
```

- `HAL_RCC_OscConfig` / `HAL_RCC_ClockConfig` 内部带**状态机与超时等待**
  （如等待 HSE RDY、PLL LOCK），失败返回 `HAL_ERROR`。
- SPL 版等价物是一串寄存器级顺序调用：
  `RCC_HSEConfig → RCC_WaitForHSEStartUp → RCC_PLLConfig → RCC_PLLCmd
  → RCC_SYSCLKConfig → RCC_HCLKConfig → RCC_PCLK1Config → RCC_PCLK2Config
  → FLASH_SetLatency`，顺序错一步即死机，且无统一返回值。

#### `Error_Handler()`

```c
__disable_irq();
while (1) { }
```

HAL 的约定：任何 `HAL_StatusTypeDef` 返回非 `HAL_OK` 即进入死循环冻结系统，
防止带病运行。SPL 库函数多返回 `void` 或 `ErrorStatus` 且常被忽略，无此统一约定。

### 3.4 支撑层：`stm32f1xx_hal_msp.c` / `stm32f1xx_it.c` / `system_stm32f1xx.c`

| 文件 | 关键函数 | 作用 | SPL 对应 |
|---|---|---|---|
| `stm32f1xx_hal_msp.c` | `HAL_MspInit()` | MSP 概念：把时钟/引脚/NVIC 等**底层资源**与**逻辑配置**分离；`HAL_Init()` 回调它使能 AFIO/PWR 时钟 | 无对应文件，时钟散写在各 init |
| `stm32f1xx_it.c` | `SysTick_Handler()` | 调用 `HAL_IncTick()` 维护 1ms 时基，是 `HAL_Delay()` 的数据来源 | 模板亦提供 it.c，但 tick 变量与递增需自写 |
| `system_stm32f1xx.c` | `SystemInit()` | CMSIS 规范：复位后时钟默认配置、`SystemCoreClock` 更新 | 两版同名同责，非 HAL 特有差异 |

### 3.5 运行时 API 对照表

| 操作 | SPL | HAL |
|---|---|---|
| 置位 / 复位 | `GPIO_SetBits()` / `GPIO_ResetBits()` | `HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET/RESET)` |
| 读输入 | `GPIO_ReadInputDataBit()` → `BitAction` | `HAL_GPIO_ReadPin()` → `GPIO_PinState` |
| 翻转 | 无直接 API：读 ODR 再写 | `HAL_GPIO_TogglePin()` 一行完成 |
| 初始化 | `RCC_APB2PeriphClockCmd()` + `GPIO_Init()` | `__HAL_RCC_GPIOx_CLK_ENABLE()` + `HAL_GPIO_Init()`（由 MX_GPIO_Init 承担） |
| 返回语义 | `BitAction`（弱类型感） | `GPIO_PinState` 强类型枚举 |
| 错误反馈 | 多 `void` | 初始化类函数返回 `HAL_StatusTypeDef` |

### 3.6 分层责任差异（为什么 HAL 版没有 driver 层）

封装与否的判据 = **HAL API 与业务语义之间的差距**：

- GPIO：差距为零（无句柄、无状态、无回调、一次调用完成）→ 不封装，bsp 直调 HAL。
- UART/I2C/SPI（后续阶段）：差距大（句柄管理、中断/DMA 策略、环形缓冲、错误重试、
  多调用组合）→ 届时重建 `driver/` 层（`drv_uart / drv_i2c / drv_spi`）。

### 3.7 配置层写法（两版同构，仅常量来源不同）

```c
/* SPL 版 config/app_config.h */
#define LED_ON_LEVEL        GPIO_LEVEL_LOW   /* 来自自定义 gpio_types.h */
#define KEY_PRESSED_LEVEL   GPIO_LEVEL_LOW
#define KEY_INPUT_PULL      GPIO_PULL_UP

/* HAL 版 config/app_config.h */
#include "main.h"                            /* 引脚宏 LED_Pin/KEY_Pin 来自生成文件 */
#define LED_PORT            LED_GPIO_Port
#define LED_PIN             LED_Pin
#define LED_ON_LEVEL        GPIO_PIN_RESET   /* HAL 原生类型 */
#define KEY_PRESSED_LEVEL   GPIO_PIN_RESET
```

业务代码因此与硬件解耦：换板子只改 CubeMX 引脚或配置宏，`bsp` 层零修改。

---

## 四、关键代码摘录（HAL 版）

`bsp/bsp_led.c`：

```c
void LED_On(void)  { HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_ON_LEVEL); }
void LED_Off(void) { HAL_GPIO_WritePin(LED_PORT, LED_PIN, LED_OFF_LEVEL); }
void LED_Toggle(void) { HAL_GPIO_TogglePin(LED_PORT, LED_PIN); }
```

`bsp/bsp_key.c`：

```c
key_state_t KEY_Read(void)
{
    if (HAL_GPIO_ReadPin(KEY_PORT, KEY_PIN) == KEY_PRESSED_LEVEL)
        return KEY_PRESSED;
    return KEY_RELEASED;
}
```

`Core/Src/main.c`（仅 USER CODE 区）：

```c
/* USER CODE BEGIN 2 */
LED_Init();
KEY_Init();
/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
if (KEY_Read() == KEY_PRESSED) { LED_On(); } else { LED_Off(); }
/* USER CODE END 3 */
```

---

## 五、验收现象

| 实验 | 现象 | 电平验证 |
|---|---|---|
| 点亮 LED | LED 常亮 | PC13 = 0（低电平点亮） |
| 按键控制 LED | 按下亮、松开灭 | 按下 PA0 = 0，松开 PA0 = 1（内部上拉） |
| 双版本对比 | 暂无 | — |

---

## 六、问题与解决

1. **按键偶发误判**：机械抖动所致；今日仅验证电平逻辑，消抖留待以后。
2. **SPL 时钟引脚无反应**：确认"使用外设必先使能时钟"；HAL 版由 MX_GPIO_Init 统一处理。

---

# Cubemx 生成代码概览
## 一、总体架构概览

这是一个 **STM32CubeMX 生成的 STM32F103C8T6（Blue Pill）工程**，配置了 LED（PC13）和 KEY（PA0）。整个目录按"分层架构"组织：

```
┌─────────────────────────────────────────────┐
│  工程配置层   .ioc / .mxproject / MDK-ARM     │  ← CubeMX + Keil 配置
├─────────────────────────────────────────────┤
│  用户代码层   Core/Inc + Core/Src             │  ← 你的业务代码（LED/KEY）
├─────────────────────────────────────────────┤
│  HAL 驱动层   Drivers/STM32F1xx_HAL_Driver/   │  ← ST 外设驱动（GPIO/RCC）
├─────────────────────────────────────────────┤
│  CMSIS 层     Drivers/CMSIS/                 │  ← ARM 内核 + 芯片寄存器定义
├─────────────────────────────────────────────┤
│  硬件         STM32F103C8T6                   │
└─────────────────────────────────────────────┘
```

---

## 二、分类详解

### A. 工程配置层（CubeMX + Keil）

| 文件 | 作用 |
|---|---|
| `led_key.ioc` | **CubeMX 图形化配置核心**。记录引脚（PC13=LED、PA0=KEY）、时钟树（HSE 8MHz→PLL 72MHz）、外设、生成选项。双击可重新打开 CubeMX 修改配置并重新生成代码。 |
| `.mxproject` | CubeMX 工程元数据。记录 MCU 型号、IP 版本、已生成文件清单，供 CubeMX 增量生成时使用。 |
| `MDK-ARM/led_key.uvprojx` | **Keil 工程文件**。定义编译选项（芯片型号、优化等级、宏）、源文件组织、输出路径。Keil 打开此文件编译工程。 |
| `MDK-ARM/led_key.uvoptx` | Keil 调试选项文件。记录调试器配置（ST-Link/J-Link）、断点、寄存器窗口设置。 |
| `MDK-ARM/startup_stm32f103xb.s` | **启动文件（汇编）**。定义向量表（中断入口地址表）、堆栈初始化、`Reset_Handler`（跳转到 main 前的 C 环境初始化）。 |

### B. 用户代码层（Core/）

| 文件 | 作用 |
|---|---|
| `Inc/main.h` / `Src/main.c` | **主程序**。`main()` 入口、`SystemClock_Config()`（HSE→PLL 72MHz）、`Error_Handler()`、引脚宏定义（`LED_Pin GPIO_PIN_13`、`KEY_Pin GPIO_PIN_0`）。 |
| `Inc/gpio.h` / `Src/gpio.c` | **GPIO 初始化**。`MX_GPIO_Init()`：LED PC13 推挽输出（初始高电平）、KEY PA0 上拉输入、GPIO 时钟使能（GPIOC/GPIOA）。 |
| `Inc/stm32f1xx_hal_conf.h` | **HAL 库配置**。定义启用哪些 HAL 模块（GPIO/RCC/CORTEX/FLASH…）、外部晶振频率宏（`HSE_VALUE 8000000`）、超时宏。 |
| `Inc/stm32f1xx_it.h` / `Src/stm32f1xx_it.c` | **中断服务程序**。`SysTick_Handler()`（1ms 心跳）、`HardFault_Handler()`、`NMI_Handler()` 等系统异常入口。 |
| `Src/stm32f1xx_hal_msp.c` | **MSP 层（MCU Support Package）**。外设底层初始化回调（`HAL_MspInit()`）：全局时钟使能、NVIC 优先级分组。 |
| `Src/system_stm32f1xx.c` | **系统初始化**。`SystemInit()`：F1 系列时钟树默认配置、向量表偏移设置、`SystemCoreClock` 变量更新。 |

### C. ST 外设驱动层（Drivers/STM32F1xx_HAL_Driver/）

#### Inc（头文件）

| 文件 | 作用 | 本工程是否用到 |
|---|---|---|
| `stm32f1xx_hal.h` | HAL 总头文件。定义 HAL 版本、全局初始化（`HAL_Init()`）、延时（`HAL_Delay()`）。 | ✅ |
| `stm32f1xx_hal_gpio.h` / `_ex.h` | GPIO 驱动。`HAL_GPIO_Init/WritePin/ReadPin/TogglePin`。 | ✅ |
| `stm32f1xx_hal_rcc.h` / `_ex.h` | RCC 时钟驱动。`HAL_RCC_OscConfig/ClockConfig`、时钟使能宏（`__HAL_RCC_GPIOC_CLK_ENABLE()`）。 | ✅ |
| `stm32f1xx_hal_cortex.h` | Cortex 内核控制。NVIC 配置、SysTick 初始化（`HAL_InitTick()`）。 | ✅ |
| `stm32f1xx_hal_def.h` | HAL 定义。宏、类型、弱定义（`__weak`）。 | ✅ |
| `stm32f1xx_hal_dma.h` / `_ex.h` | DMA 驱动。 | ❌（HAL 包自带） |
| `stm32f1xx_hal_exti.h` | 外部中断驱动。 | ❌ |
| `stm32f1xx_hal_flash.h` / `_ex.h` | Flash 驱动。 | ❌ |
| `stm32f1xx_hal_pwr.h` | 电源驱动。 | ❌ |
| `Legacy/stm32_hal_legacy.h` | 旧版 API 兼容层。 | ❌ |

#### Src（实现）

与 Inc 一一对应的 `.c` 实现（`stm32f1xx_hal_gpio.c`、`stm32f1xx_hal_rcc.c` 等）。本工程实际编译链接的只有 `hal.c`、`hal_gpio.c`、`hal_rcc.c`、`hal_cortex.c`。

### D. CMSIS 层（Drivers/CMSIS/）

#### Device/ST/STM32F1xx/（芯片寄存器定义）

| 文件 | 作用 |
|---|---|
| `Include/stm32f103xb.h` | **芯片寄存器定义**。具体型号 STM32F103xB 的寄存器映射（GPIOA_BASE、RCC_BASE…）、中断号（IRQn）、位域定义。 |
| `Include/stm32f1xx.h` | 芯片系列总头文件。根据型号宏（`STM32F103xB`）选择包含 `stm32f103xb.h`。 |
| `Include/system_stm32f1xx.h` | 系统初始化头文件（`SystemInit()`、`SystemCoreClock` 声明）。 |
| `Source/Templates/` | 系统初始化模板（`system_stm32f1xx.c` 的模板，CubeMX 复制到 Core/Src）。 |

#### Include/（ARM 内核头文件）

| 文件 | 作用 | 本工程是否用到 |
|---|---|---|
| `core_cm3.h` | **Cortex-M3 内核头文件**。内核寄存器（NVIC、SysTick、SCB）、内联函数（`__NVIC_EnableIRQ()`）。本工程芯片是 Cortex-M3。 | ✅ |
| `cmsis_compiler.h` / `cmsis_gcc.h` / `cmsis_armcc.h` | 编译器适配层。根据编译器（GCC/ARMCC）选择内联汇编实现。 | ✅ |
| `cmsis_version.h` | CMSIS 版本定义。 | ✅ |
| `core_cm0/cm4/cm7/m33…` | 其他内核头文件（CMSIS 包自带，本工程不用）。 | ❌ |
| `tz_context.h` | TrustZone 上下文（M33 用，本工程不用）。 | ❌ |

### E. 启动与系统层

| 文件 | 作用 |
|---|---|
| `MDK-ARM/startup_stm32f103xb.s` | 启动文件。向量表 + 堆栈初始化 + `Reset_Handler`。 |
| `Core/Src/system_stm32f1xx.c` | 系统初始化。`SystemInit()` + `SystemCoreClock` 更新。 |

---

## 三、各层依赖关系

```
main.c（用户代码）
  ↓ 调用 HAL_GPIO_WritePin / HAL_RCC_OscConfig
stm32f1xx_hal_gpio.c / hal_rcc.c（HAL 驱动）
  ↓ 操作寄存器宏（GPIOC->ODR、RCC->CR）
stm32f103xb.h / core_cm3.h（CMSIS 寄存器定义）
  ↓
硬件寄存器
```

加上配置层：

```
led_key.ioc（CubeMX 配置）
  ↓ 生成
Core/ + Drivers/ + MDK-ARM/（代码 + 工程）
  ↓ 编译
led_key.hex / .bin（固件）
  ↓ 烧录
STM32F103C8T6
```

---

## 四、总结

1. **生成逻辑**：`led_key.ioc` 是配置源头，CubeMX 根据它生成 `Core/`（用户代码）、`Drivers/`（HAL + CMSIS）、`MDK-ARM/`（Keil 工程）。修改 `.ioc` 后重新生成，`USER CODE BEGIN/END` 之间的用户代码会被保留。

2. **本工程实际用到的文件**：
   - 用户代码：`main.c/h`、`gpio.c/h`、`stm32f1xx_it.c/h`、`stm32f1xx_hal_msp.c`、`system_stm32f1xx.c`、`stm32f1xx_hal_conf.h`。
   - HAL：`hal.c/h`、`hal_gpio.c/h`、`hal_rcc.c/h`、`hal_cortex.c/h`。
   - CMSIS：`stm32f103xb.h`、`stm32f1xx.h`、`core_cm3.h`、`cmsis_compiler.h`。
   - 启动：`startup_stm32f103xb.s`。

3. **可裁剪/未用到的文件**：`Drivers/` 中的 DMA/EXTI/FLASH/PWR 驱动、CMSIS 中的其他内核头文件（cm0/cm4/cm7/m33），都是 HAL/CMSIS 包自带，本工程不编译链接，保留即可（不影响固件大小）。

4. **分层价值**：用户代码只调 HAL API（`HAL_GPIO_WritePin`），HAL 只操作 CMSIS 寄存器宏，CMSIS 只映射硬件寄存器。每层隔离，移植时只需替换 CMSIS/HAL 层，用户代码不变。

> **一句话总结**：`.ioc` 是配置源头，`Core/` 是业务代码，`Drivers/STM32F1xx_HAL_Driver/` 是外设驱动，`Drivers/CMSIS/` 是内核+寄存器定义，`MDK-ARM/` 是编译工程——五层协作，从配置到固件到硬件。
