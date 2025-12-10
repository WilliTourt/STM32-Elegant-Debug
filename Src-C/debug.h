#ifndef STM32_ELEGANT_DEBUG_C_H
#define STM32_ELEGANT_DEBUG_C_H

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

#ifdef __cplusplus
extern "C" {
#endif

/* ANSI escape codes for output *****************************************/

/* Color/style macros */
#define CLR                 "\033[0m"

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

/* Background colors and styles */
#define BG_RED              "\033[41m"
#define BG_GREEN            "\033[42m"
#define BG_YELLOW           "\033[43m"
#define BG_BLUE             "\033[44m"

#define BOLD                "\033[1m"
#define UNDERLINE           "\033[4m"
#define BLINK               "\033[5m"

/* Prefixes */
#define ERROR_TYPE          "\033[91m\033[1m[ ERROR ]\033[0m "
#define WARNING_TYPE        "\033[93m\033[1m[ WARNING ]\033[0m "
#define INFO_TYPE           "\033[94m\033[1m[ INFO ]\033[0m "
#define OK_TYPE             "\033[92m\033[1m[ OK ]\033[0m "
#define SUCCESS_TYPE        "\033[92m\033[1m[ SUCCESS ]\033[0m "

#define ERROR_TYPE_PLAIN    "[ ERROR ] "
#define WARNING_TYPE_PLAIN  "[ WARNING ] "
#define INFO_TYPE_PLAIN     "[ INFO ] "
#define OK_TYPE_PLAIN       "[ OK ] "
#define SUCCESS_TYPE_PLAIN  "[ SUCCESS ] "

/************************************************************************/

#define DEBUG_BUFFER_LEN 256

/* Initialize the library; MUST be called before other functions. */
void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color);

/* Basic formatted log */
void debug_log(const char* format, ...);

/* Log with a type prefix */
void debug_logWithType(const char* type, const char* format, ...);

/* Convenience helpers */
void debug_error(const char* format, ...);
void debug_warning(const char* format, ...);
void debug_ok(const char* format, ...);
void debug_success(const char* format, ...);
void debug_info(const char* format, ...);

/* Runtime setters */
void debug_setTimestampEnabled(bool enabled);
void debug_setColorEnabled(bool enabled);

#ifdef __cplusplus
}
#endif

#endif /* STM32_ELEGANT_DEBUG_C_H */
