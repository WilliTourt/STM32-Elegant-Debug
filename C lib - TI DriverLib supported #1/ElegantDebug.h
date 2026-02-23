/*******************************************************************************
 * @file    ElegantDebug.h
 * @version 1.1-mspm0-tickptr
 * @brief   C API for ANSI-colored debug logging on TI MSPM0 (MSPM0G3507).
 *
 * Port notes:
 * - Removed STM32 HAL dependency (UART_HandleTypeDef / HAL_UART_Transmit / HAL_GetTick).
 * - UART send uses MSPM0 DriverLib (DL_UART_Main_transmitData).
 * - Timestamp uses user-provided tick pointer (volatile uint32_t*).
 *
 * Usage:
 *   extern volatile uint32_t g_msTick;   // maintained by SysTick/Timer ISR
 *   debug_init(UART_0_INST, &g_msTick, true, true, false);
 *   debug_info("Hello\r\n");
 ******************************************************************************/

#ifndef __ELEGANT_DEBUG_H
#define __ELEGANT_DEBUG_H

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* SysConfig 生成：包含 UART_0_INST 等实例宏 */
#include "ti_msp_dl_config.h"

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

#define COLOR_CUSTOM(r,g,b) "\033[38;2;" _ED_STR(r) ";" _ED_STR(g) ";" _ED_STR(b) "m"

// Background colors
#define BG_RED              "\033[41m"
#define BG_GREEN            "\033[42m"
#define BG_YELLOW           "\033[43m"
#define BG_BLUE             "\033[44m"
#define BG_MAGENTA          "\033[45m"
#define BG_CYAN             "\033[46m"
#define BG_WHITE            "\033[47m"

#define BG_COLOR_CUSTOM(r,g,b) "\033[48;2;" _ED_STR(r) ";" _ED_STR(g) ";" _ED_STR(b) "m"

// Styles
#define BOLD                "\033[1m"
#define DIM                 "\033[2m"
#define ITALIC              "\033[3m"
#define UNDERLINE           "\033[4m"
#define BLINK               "\033[5m"
#define REVERSE             "\033[7m"
#define CONCEAL             "\033[8m"

// Clear specific color/styles
#define CLR                 "\033[0m"
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

/**
 * @brief Initialize the library; MUST be called before other functions.
 *
 * @param uart_base UART instance base address (e.g. UART_0_INST from ti_msp_dl_config.h)
 * @param tick_ptr  Pointer to a millisecond tick counter (volatile uint32_t*).
 *                  - If NULL, timestamp prints as 00:00:00.000
 *                  - You maintain this variable in SysTick/Timer ISR.
 */
void debug_init(UART_Regs * uart_base,
                volatile uint32_t *tick_ptr,
                bool enable_timestamp,
                bool enable_color,
                bool enable_filename_line);

// Basic formatted log
void debug_log(const char* format, ...);

// Log with a type prefix
void debug_logWithType(const char* type, const char* style, const char* format, ...);

// Convenience helpers
void debug_ok(const char* format, ...);
void debug_success(const char* format, ...);
void debug_info(const char* format, ...);

// Variants that accept file and line
void debug_error_fileline(const char* file, int line, const char* format, ...);
void debug_warning_fileline(const char* file, int line, const char* format, ...);

// Runtime setters
void debug_setTimestampEnabled(bool enabled);
void debug_setColorEnabled(bool enabled);
void debug_setFilenameLineEnabled(bool enabled);

// Macros to automatically pass caller file/line
#define debug_error(...)                debug_error_fileline(__FILE__, __LINE__, __VA_ARGS__)
#define debug_warning(...)              debug_warning_fileline(__FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // __ELEGANT_DEBUG_H
