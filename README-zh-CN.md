# STM32 Elegant Debug

> **Language/语言**: [English](README.md) | [简体中文](README-zh-CN.md)

## 简介
- 这是一个轻量的串口调试库，C / C++ 均适用，适配STM32 HAL库，提供格式化日志输出功能。
- 特点：格式化日志、可选的时间戳、可选的 ANSI 颜色输出、便捷的类型前缀（ERROR/WARNING/INFO/OK/SUCCESS）。

![Example](example.png)

## 文件结构（仓库）
- C 版本放在 `Src-C/`
- 另有 C++ 版本放在 `Src-CPP/`

## 快速开始
- 依赖 STM32Cube HAL 驱动，必须启用一个串口
- 输出长度由 `DEBUG_BUFFER_LEN` 宏控制（默认 256）

1. 将 `Src-C/debug.h` 与 `Src-C/debug.c` 添加到你的工程（确保 `debug.c` 被编译并加入链接）。
   - Makefile/CMake：将 `.../Src-C/debug.c` 加入 `SRCS` / `target_sources`。
   - Keil/MDK / IAR / CubeIDE：在 Project Explorer 中 Right-click -> Add Existing Files。

2. 在你的代码中包含头文件并初始化（以 `huart1` ，C语言版本为例，C++版本用法完全一致）：
```c
#include "debug.h"

/* 在合适的位置（比如 main() 里）初始化 */
debug_init(&huart1, true /* enable timestamp */, true /* enable color */);

/* 使用示例 */
debug_log("Hello world！");
debug_info("System started. Free memory: %d bytes", free_mem);
debug_warning("Low battery: %.2f %%", battery_percentage);
debug_error("Sensor failed: code = %d", err_code);
debug_ok("Operation finished in %d ms", elapsed_ms);
debug_success("Configuration saved");
debug_logWithType("[CUSTOM]", "custom message: %s", "ok");

/* 颜色和文本样式 */
debug_log("%sI am GREEN!%s\n", COLOR_GREEN, CLR);
debug_log("%sI have a colored background!%s\n", BG_RED, CLR);
debug_log("%sI am underlined and %spartly colored!%s%s\n", UNDERLINE, COLOR_DARK_MAGENTA, CLR, CLR);
```

## API
### C 版本
- `void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color);`
  - 初始化库，必须先调用，传入 HAL UART 句柄和是否启用时间戳/颜色。
- `void debug_log(const char* format, ...);`
  - 原始格式化输出（无前缀）。
- `void debug_logWithType(const char* type, const char* format, ...);`
  - 自定义前缀的输出（例如 `"[ CUSTOM ]"`）。
- 便捷类型输出：
  - `void debug_error(const char* format, ...);`
  - `void debug_warning(const char* format, ...);`
  - `void debug_ok(const char* format, ...);`
  - `void debug_success(const char* format, ...);`
  - `void debug_info(const char* format, ...);`
- 运行时设置：
  - `void debug_setTimestampEnabled(bool enabled);`
  - `void debug_setColorEnabled(bool enabled);`

### C++ 版本
- `DEBUG(UART_HandleTypeDef *huart, bool enable_timestamp = true, bool enable_color = true);`
  - 构造函数，传入 HAL UART 句柄和是否启用时间戳/颜色。
- `void log(const char* format, ...);`
  - 原始格式化输出（无前缀）。
- `void logWithType(const char* type, const char* format, ...);`
  - 自定义前缀的输出（例如 `"[ CUSTOM ]"`）。
- 便捷类型输出：
  - `void error(const char* format, ...);`
  - `void warning(const char* format, ...);`
  - `void ok(const char* format, ...);`
  - `void success(const char* format, ...);`
  - `void info(const char* format, ...);`
- 运行时设置：
  - `void setTimestampEnabled(bool enabled);`
  - `void setColorEnabled(bool enabled);`

## 常见问题与排查
- 链接错误（undefined reference to `debug_init` / `debug_info`）：
  - 确认 `Src-C/debug.c` 已被加入到工程并被编译产生 `.o`，最终与其它目标一起链接。
- 串口无输出或者一坨乱码：
  - 确认 `huart` 已正确初始化（**特别是波特率**）。
  - 如果启用颜色但终端不支持 ANSI，可以通过 `debug_setColorEnabled(false)` 关闭颜色。

- 异常重启
  - 可能是字符串溢出，检查`DEBUG_BUFFER_LEN`缓冲区长度（默认 256）。如需更长信息，在 `debug.h` 中调整此宏（注意内存占用）

## 其他

> 此库灵感最初源于学长Zodiak_Jealously的提议 ;p

WilliTourt 2025.12.10 | willitourt@doxmail.com

欢迎提交 PR 和 Issue！！
