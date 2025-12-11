# STM32 Elegant Debug

> **Language/语言**: [English](README.md) | [简体中文](README-zh-CN.md)

## 简介
- 这是一个轻量的串口调试库，C / C++ 均适用，适配STM32 HAL库，提供格式化日志输出功能。
- 特点：
  - 格式化日志
  - 可选的时间戳
  - 可选的 ERROR/WARNING 信息 显示报错文件名和行号
  - 可选的 ANSI 颜色输出，包括完全自定义的24位颜色
  - 便捷的类型前缀（ERROR/WARNING/INFO/OK/SUCCESS）

![Example](example.png)
![Showing Filename and Line](example_fileline.png)

## 文件结构（仓库）
- C 版本放在 `Src-C/`
- 另有 C++ 版本放在 `Src-CPP/`

## 快速开始
- 依赖 STM32Cube HAL 驱动，必须启用一个串口
- 输出长度由 `DEBUG_BUFFER_LEN` 宏控制（默认 256）

1. 将 `Src-C/ElegantDebug.h` 与 `Src-C/ElegantDebug.c` 添加到你的工程（确保 `ElegantDebug.c` 被编译并加入链接）。
   - Makefile/CMake：将 `.../Src-C/ElegantDebug.c` 加入 `SRCS` / `target_sources`。
   - Keil/MDK / IAR / CubeIDE：在 Project Explorer 中 Right-click -> Add Existing Files。

2. 在你的代码中包含头文件并初始化（以 `huart1` ，C语言版本为例，C++版本用法完全一致）：
```c
#include "ElegantDebug.h"

/* 在合适的位置（比如 main() 里）初始化 */
debug_init(&huart1, true /* enable timestamp */, true /* enable color */, false /* enable filename line */);

/* 使用示例 */
debug_log("Hello world！");
debug_info("System started. Free memory: %d bytes", free_mem);
debug_warning("Low battery: %.2f %%", battery_percentage);  // 自动包含文件名和行号（如果启用）
debug_error("Sensor failed: code = %d", err_code);          // 自动包含文件名和行号（如果启用）
debug_ok("Operation finished in %d ms", elapsed_ms);
debug_success("Configuration saved");
debug_logWithType("[CUSTOM]", "custom message: %s", "ok");

/* 颜色和文本样式 */
debug_log("%sI am GREEN!%s\n", COLOR_GREEN, CLR);
debug_log("%sI have a colored background!%s\n", BG_RED, CLR);
debug_log("%sI am underlined and %spartly colored!%s%s\n", UNDERLINE, COLOR_DARK_MAGENTA, CLR, CLR);

/* 启用ERROR/WARNING信息的文件名和行号显示 */
debug_setFilenameLineEnabled(true);  // 启用后，error和warning信息将显示文件名和行号
```

## API
### C 版本
- `void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color, bool enable_filename_line);`
  - 初始化库，必须先调用，传入 HAL UART 句柄和是否启用时间戳/颜色/文件名行号显示。
- `void debug_log(const char* format, ...);`
  - 原始格式化输出（无前缀）。
- `void debug_logWithType(const char* type, const char* format, ...);`
  - 自定义前缀的输出（例如 `"[ CUSTOM ]"`）。
- 便捷类型输出（自动包含文件名和行号）：
  - `void debug_error(const char* format, ...);`
  - `void debug_warning(const char* format, ...);`
- 便捷类型输出（无文件名行号）：
  - `void debug_ok(const char* format, ...);`
  - `void debug_success(const char* format, ...);`
  - `void debug_info(const char* format, ...);`
- 运行时设置：
  - `void debug_setTimestampEnabled(bool enabled);`
  - `void debug_setColorEnabled(bool enabled);`
  - `void debug_setFilenameLineEnabled(bool enabled);`

### C++ 版本
- `ElegantDebug(UART_HandleTypeDef *huart, bool enable_timestamp = true, bool enable_color = true, bool enable_filename_line = false);`
  - 构造函数，传入 HAL UART 句柄和是否启用时间戳/颜色/文件名行号显示。
- `void log(const char* format, ...);`
  - 原始格式化输出（无前缀）。
- `void logWithType(const char* type, const char* format, ...);`
  - 自定义前缀的输出（例如 `"[ CUSTOM ]"`）。
- 便捷类型输出（C++20及以上版本自动包含文件名和行号）：
  - `void error(const char* format, ...);` (C++20: 支持 `std::source_location`)
  - `void warning(const char* format, ...);` (C++20: 支持 `std::source_location`)
- 便捷类型输出（无文件名行号）：
  - `void ok(const char* format, ...);`
  - `void success(const char* format, ...);`
  - `void info(const char* format, ...);`
- 运行时设置：
  - `void setTimestampEnabled(bool enabled);`
  - `void setColorEnabled(bool enabled);`
  - `void setFilenameLineEnabled(bool enabled);` (仅C++20及以上版本)

### 关于ANSI转义码
库中的颜色和样式宏均为 ANSI 转义码，如果终端不支持，可以通过运行时设置关闭颜色输出，并停用%s修饰的操作。

要为你的输出字符串设置自定义颜色和样式，请用一对 `%s` 来包裹颜色/样式宏和用以清除颜色/样式的宏。例如：
```cpp
log("Hello World! %sI am RED!%s %s%sI am italic and GREEN!%s Now i cleared italic style but still GREEN.%s\n", 
COLOR_RED, CLR_TEXT_COLOR, ITALIC, COLOR_GREEN, CLR_ITALIC, CLR);
```

对于24位自定义颜色，请使用 `COLOR_CUSTOM(r,g,b)` 和 `BG_COLOR_CUSTOM(r,g,b)` 宏，其中 `r`、`g`、`b` 为0~255之间的整数。

关于 ANSI 转义码的更多信息，请参考：[ANSI escape code - Handwiki](https://handwiki.org/wiki/ANSI_escape_code)

## 常见问题与排查
- 链接错误（undefined reference to `debug_init` / `debug_info`）：
  - 确认 `Src-C/ElegantDebug.c` 已被加入到工程并被编译产生 `.o`，最终与其它目标一起链接。
- 串口无输出或者一坨乱码：
  - 确认 `huart` 已正确初始化（**特别是波特率**）。
  - 如果启用颜色但终端不支持 ANSI，可以通过 `debug_setColorEnabled(false)` 关闭颜色。

- 异常重启
  - 可能是字符串溢出，检查`DEBUG_BUFFER_LEN`缓冲区长度（默认 256）。如需更长信息，在 `debug.h` 中调整此宏（注意内存占用）

## 更新日志

### v1.0 (2025-12-10)
- **初始发布**: STM32 Elegant Debug 库首次发布
- **功能**: 格式化日志输出、时间戳、ANSI颜色、类型前缀等基础功能
- **支持**: C和C++双版本，适配STM32 HAL库

### v1.1 (2025-12-11)
- **新增**: 支持在错误和警告信息中显示文件名和行号
  - C版本：通过 `debug_setFilenameLineEnabled(true)` 启用，`debug_error()` 和 `debug_warning()` 自动包含文件名行号
  - C++版本（C++20及以上）：通过 `setFilenameLineEnabled(true)` 启用，支持 `std::source_location`
- **新增**: 支持更多的ANSI颜色和样式
- **改进**: 更新API文档，更准确地反映实际功能

## 其他

> 此库灵感最初源于学长Zodiak_Jealously的提议 ;p

WilliTourt 2025.12.10 initial release | willitourt@doxmail.com

欢迎提交 PR 和 Issue！！
