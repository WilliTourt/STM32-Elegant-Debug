/*******************************************************************************
 * @file    ElegantDebug.h
 * @version 1.2
 * @brief   C API for ANSI-colored debug logging on STM32 (header).
 *
 * This header provides a small C-friendly API that mirrors the C++
 * `ElegantDebug` class. It sends formatted, optionally colored and timestamped
 * messages through a HAL `UART_HandleTypeDef` instance.
 *
 * Requirements:
 *  - STM32Cube HAL UART driver must be available and `HAL_UART_MODULE_ENABLED`
 *    must be defined in your build.
 *  - Include this header after your MCU/board `main.h` which provides the
 *    `UART_HandleTypeDef` declaration and HAL prototypes.
 *
 * Example:
 *   debug_init(&huart1, true, true);
 *   debug_info("Hello from STM32!\n");
 *
 * @author:    WilliTourt <willitourt@foxmail.com>
 * @date:      2026-02-22
 * 
 * @changelog:
 * - 2025-12-10: Initial release.
 * - 2025-12-11: Added support for filename and ln number in warning/error messages.
 *               But this feature is not available below C++20.
 *               Allowed custom styles for type prefix in logWithType().
 *               Added more ANSI escape codes for color/style.
 * - 2026-02-22: Added USB-CDC support; can redirect output to USB when
 *               USB_AS_DEBUG_PORT macro set to 1. See C++ counterpart for
 *               details.
 * 
 ******************************************************************************/

#ifndef __ELEGANT_DEBUG_H
#define __ELEGANT_DEBUG_H



// Set this to 1 to route log output over the USB‑CDC port instead of the
// UART peripheral. Requires STM32CubeMX generated USB device stack and a
// definition of `CDC_Transmit_FS` (usually provided by `usbd_cdc_if.h`).
// Leave at 0 (or `false`) to continue using HAL_UART_Transmit().
#define USB_AS_DEBUG_PORT false



#include "main.h"

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if !defined(__STM32F1xx_HAL_H) && \
    !defined(__STM32F4xx_HAL_H) && \
    !defined(__STM32F7xx_HAL_H) && \
    !defined(__STM32H7xx_HAL_H) && \
    !defined(__STM32L4xx_HAL_H) && \
    !defined(__STM32G0xx_HAL_H) && \
    !defined(__STM32WBxx_HAL_H) && \
    !defined(__STM32WLxx_HAL_H)
#error "This debugging library can only be used with STM32Cube HAL drivers"
#endif

#if !defined(HAL_UART_MODULE_ENABLED)
#error "At least one serial port should be opened"
#endif

#if (USB_AS_DEBUG_PORT == 1)
    #include "usbd_cdc_if.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ANSI escape codes for output *****************************************/

// Colors for texts
#define COLOR_DARK_RED      "\033[31m"
#define COLOR_DARK_GREEN    "\033[32m"
#define COLOR_DARK_YELLOW   "\033[33m"
#define COLOR_DARK_BLUE     "\033[34m"
#define COLOR_DARK_MAGENTA  "\033[35m"
#define COLOR_DARK_CYAN     "\033[36m"
#define COLOR_DARK_WHITE    "\033[37m"

#define COLOR_RED           "\033[91m"
#define COLOR_GREEN         "\033[92m"
#define COLOR_YELLOW        "\033[93m"
#define COLOR_BLUE          "\033[94m"
#define COLOR_MAGENTA       "\033[95m"
#define COLOR_CYAN          "\033[96m"
#define COLOR_WHITE         "\033[97m"

// Helper to stringify macro arguments (two-step to allow macro expansion)
#define _ED_STR_HELPER(x) #x
#define _ED_STR(x) _ED_STR_HELPER(x)

#define COLOR_CUSTOM(r,g,b) "\033[38;2;" _ED_STR(r) ";" _ED_STR(g) ";" _ED_STR(b) "m" // Custom 24-bit colors for text

// Background colors
#define BG_RED              "\033[41m"
#define BG_GREEN            "\033[42m"
#define BG_YELLOW           "\033[43m"
#define BG_BLUE             "\033[44m"
#define BG_MAGENTA          "\033[45m"
#define BG_CYAN             "\033[46m"
#define BG_WHITE            "\033[47m"

#define BG_COLOR_CUSTOM(r,g,b) "\033[48;2;" _ED_STR(r) ";" _ED_STR(g) ";" _ED_STR(b) "m" // Custom 24-bit background colors

// Styles
#define BOLD                "\033[1m"
#define DIM                 "\033[2m"
#define ITALIC              "\033[3m"
#define UNDERLINE           "\033[4m"
#define BLINK               "\033[5m"
#define REVERSE             "\033[7m"
#define CONCEAL             "\033[8m"

// Clear specific color/styles
#define CLR                 "\033[0m"   // Clear color and style of text before this macro
#define CLR_TEXT_COLOR      "\033[39m"
#define CLR_BG_COLOR        "\033[49m"
#define CLR_BOLD            "\033[21m"
#define CLR_DIM             "\033[22m"
#define CLR_ITALIC          "\033[23m"
#define CLR_UNDERLINE       "\033[24m"
#define CLR_BLINK           "\033[25m"
#define CLR_REVERSE         "\033[27m"
#define CLR_CONCEAL         "\033[28m"

// Prefixes
#define ERROR_TYPE          "\033[91m\033[1m[ERROR]\033[0m "
#define WARNING_TYPE        "\033[93m\033[1m[WARNING]\033[0m "
#define INFO_TYPE           "\033[94m\033[1m[INFO]\033[0m "
#define OK_TYPE             "\033[92m\033[1m[OK]\033[0m "
#define SUCCESS_TYPE        "\033[92m\033[1m[SUCCESS]\033[0m "

#define ERROR_TYPE_PLAIN    "[ERROR] "
#define WARNING_TYPE_PLAIN  "[WARNING] "
#define INFO_TYPE_PLAIN     "[INFO] "
#define OK_TYPE_PLAIN       "[OK] "
#define SUCCESS_TYPE_PLAIN  "[SUCCESS] "

/************************************************************************/

#define DEBUG_BUFFER_LEN 256

// Initialize the library; MUST be called before other functions.
//
// If `USB_AS_DEBUG_PORT` is set to 1 (USB-CDC output enabled at compile time),
// the `huart` pointer is ignored and may be passed as NULL. When using UART as
// the debug port, a valid `UART_HandleTypeDef *` must be provided or the
// functions will no-op.
void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color, bool enable_filename_line);

// Basic formatted log
void debug_log(const char* format, ...);

// Log with a type prefix
void debug_logWithType(const char* type, const char* style, const char* format, ...);

// Convenience helpers
void debug_ok(const char* format, ...);
void debug_success(const char* format, ...);
void debug_info(const char* format, ...);

// Variants that accept file and line (used by macros to automatically pass __FILE__/__LINE__)
// void debug_logWithType_fileline(const char* file, int line, const char* type, const char* format, ...);
void debug_error_fileline(const char* file, int line, const char* format, ...);
void debug_warning_fileline(const char* file, int line, const char* format, ...);

// Runtime setters
void debug_setTimestampEnabled(bool enabled);
void debug_setColorEnabled(bool enabled);
// Enable/disable showing filename:line when using the file/line variants or macros
void debug_setFilenameLineEnabled(bool enabled);

// Macros to automatically pass caller file/line
#define debug_error(...)                debug_error_fileline(__FILE__, __LINE__, __VA_ARGS__)
#define debug_warning(...)              debug_warning_fileline(__FILE__, __LINE__, __VA_ARGS__)
// #define debug_logWithType(type, ...) debug_logWithType_fileline(__FILE__, __LINE__, (type), __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __ELEGANT_DEBUG_H
