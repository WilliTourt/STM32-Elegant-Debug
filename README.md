# STM32 Elegant Debug

> **Language/语言**: [English](README.md) | [简体中文](README-zh-CN.md)

## Introduction
- A lightweight serial/USB-CDC debug library suitable for both C and C++ projects on STM32 using the HAL drivers.
- Features: 
  - Formatted output
  - Optional timestamp
  - Optional filename and line number display for ERROR/WARNING messages
  - Optional ANSI color output, including fully customizable 24-bit colors
  - Convenient type prefixes (ERROR/WARNING/INFO/OK/SUCCESS)

![Example](example.png)
![Showing Filename and Line](example_fileline.png)

## Repository layout
- C implementation is in `Src-C/`
- C++ implementation is in `Src-CPP/`

## Quick Start
- Depends on STM32Cube HAL and at least one UART enabled, or USB-CDC enabled (enable USB_DEVICE middleware in MX and set to CDC class).
- The output buffer length is controlled by the `DEBUG_BUFFER_LEN` macro (default 256).
- Output method (UART/USB) is controlled by the `USB_AS_DEBUG_PORT` macro (default 0 for UART; set to 1 for USB-CDC).

1. Add `Src-C/ElegantDebug.h` and `Src-C/ElegantDebug.c` to your project (make sure `ElegantDebug.c` is compiled and linked).
   - Makefile/CMake: add `.../Src-C/ElegantDebug.c` to your `SRCS` / `target_sources`.
   - Keil/MDK / IAR / CubeIDE: in the Project Explorer, Right-click -> Add Existing Files.

2. Include the header and initialize (example using `huart1`, C API shown; C++ usage is identical):
```c
#include "ElegantDebug.h"

/* Initialize at an appropriate place (e.g. in main()) */
// When USB-CDC is configured as the debug port (compile-time macro
// `USB_AS_DEBUG_PORT` set to 1), the `huart` argument can be NULL and 
// will be ignored by the library.
debug_init(&huart1, true /* enable timestamp */, true /* enable color */, false /* enable filename line */);

/* Usage examples */
debug_log("Hello world!");
debug_info("System started. Free memory: %d bytes", free_mem);
debug_warning("Low battery: %.2f %%", battery_percentage);  // Automatically includes filename and line number (if enabled)
debug_error("Sensor failed: code = %d", err_code);          // Automatically includes filename and line number (if enabled)
debug_ok("Operation finished in %d ms", elapsed_ms);
debug_success("Configuration saved");
debug_logWithType("[CUSTOM]", "custom message: %s", "ok");

/* Colors and styles */
debug_log("%sI am GREEN!%s\n", COLOR_GREEN, CLR);
debug_log("%sI have a colored background!%s\n", BG_RED, CLR);
debug_log("%sI am underlined and %spartly colored!%s%s\n", UNDERLINE, COLOR_DARK_MAGENTA, CLR, CLR);

/* Enable filename and line number display (optional) */
debug_setFilenameLineEnabled(true);  // After enabling, error and warning messages will show filename and line number
```

## API
### C API
- `void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color, bool enable_filename_line);`
  - Initialize the library. Must be called before other functions. Provide a HAL UART handle and flags to enable timestamp/color/filename-line display.
- `void debug_log(const char* format, ...);`
  - Basic formatted output (no prefix).
- `void debug_logWithType(const char* type, const char* format, ...);`
  - Output with a custom type prefix (e.g. `"[ CUSTOM ]"`).
- Convenience helpers (automatically include filename and line number):
  - `void debug_error(const char* format, ...);`
  - `void debug_warning(const char* format, ...);`
- Convenience helpers (no filename/line):
  - `void debug_ok(const char* format, ...);`
  - `void debug_success(const char* format, ...);`
  - `void debug_info(const char* format, ...);`
- Runtime setters:
  - `void debug_setTimestampEnabled(bool enabled);`
  - `void debug_setColorEnabled(bool enabled);`
  - `void debug_setFilenameLineEnabled(bool enabled);`

### C++ API
- `ElegantDebug(UART_HandleTypeDef *huart, bool enable_timestamp = true, bool enable_color = true, bool enable_filename_line = false);`
  - Constructor: pass a HAL UART handle and flags to enable timestamp/color/filename-line display.
- `void log(const char* format, ...);`
- `void logWithType(const char* type, const char* format, ...);`
- Convenience helpers (C++20 and above automatically include filename and line number):
  - `void error(const char* format, ...);` (C++20: supports `std::source_location`)
  - `void warning(const char* format, ...);` (C++20: supports `std::source_location`)
- Convenience helpers (no filename/line):
  - `void ok(const char* format, ...);`
  - `void success(const char* format, ...);`
  - `void info(const char* format, ...);`
- Runtime setters:
  - `void setTimestampEnabled(bool enabled);`
  - `void setColorEnabled(bool enabled);`
  - `void setFilenameLineEnabled(bool enabled);` (C++20 and above only)

### About ANSI Escape Codes
The color and style macros in the library are ANSI escape codes. If your terminal doesn't support them, you can disable color output at runtime and avoid using the `%s` formatting operations.

To set custom colors and styles for your output strings, wrap the color/style macros and the clear macros with a pair of `%s` placeholders. For example:
```cpp
log("Hello World! %sI am RED!%s %s%sI am italic and GREEN!%s Now i cleared italic style but still GREEN.%s\n", 
COLOR_RED, CLR_TEXT_COLOR, ITALIC, COLOR_GREEN, CLR_ITALIC, CLR);
```

For 24-bit custom colors, use the `COLOR_CUSTOM(r,g,b)` and `BG_COLOR_CUSTOM(r,g,b)` macros, where `r`, `g`, `b` are integers between 0 and 255.

For more information about ANSI escape codes, refer to: [ANSI escape code - Handwiki](https://handwiki.org/wiki/ANSI_escape_code)

## Troubleshooting
- Linker errors (undefined reference to `debug_init` / `debug_info`):
  - Ensure `Src-C/ElegantDebug.c` is added to the project and compiled into an object file that is linked into your final binary.
- No output or garbled output on the serial terminal:
  - Verify `huart` is correctly initialized (especially baud rate).
  - If colors are enabled but your terminal does not support ANSI, call `debug_setColorEnabled(false)`.
- Unexpected resets/crashes:
  - Possible buffer overflow: check `DEBUG_BUFFER_LEN` (default 256). Increase it in `ElegantDebug.h` if you need longer messages (mind RAM usage).

## Changelog

### v1.0 (2025-12-10)
- **Initial release**: First release of STM32 Elegant Debug library
- **Features**: Formatted log output, timestamp, ANSI colors, type prefixes and other basic functionality
- **Support**: Dual C and C++ versions, compatible with STM32 HAL library

### v1.1 (2025-12-11)
- **New**: Support for displaying filename and line number in error and warning messages
  - C version: Enable with `debug_setFilenameLineEnabled(true)`, `debug_error()` and `debug_warning()` automatically include filename and line number
  - C++ version (C++20 and above): Enable with `setFilenameLineEnabled(true)`, supports `std::source_location`
- **New**: Supports more ANSI colors and styles
- **Improvement**: Updated API documentation to accurately reflect actual functionality

### v1.2 (2026-02-22)
- **New**: This library now supports USB-CDC output. To use this feature, simply enable the USB_DEVICE middleware in your MX configuration and set USB to CDC class. Then define `USB_AS_DEBUG_PORT` to `1` in the library's header file.

## License
- See the `LICENSE` file at the repository root for licensing details.

## Other

> This project was inspired by a suggestion from Zodiak_Jealously ;p

WilliTourt 2025.12.10 initial release | willitourt@doxmail.com

Contributions and PRs welcome!!
