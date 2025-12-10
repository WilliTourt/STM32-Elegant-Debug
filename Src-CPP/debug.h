#/*******************************************************************************
 * @file    debug.h
 * @brief   ANSI-colored debug logging for STM32 (C++ header).
 *
 * This header declares the C++ `DEBUG` class used to send formatted log
 * messages over a HAL UART interface. The class supports optional timestamps
 * and ANSI color output for terminals that support it. A functionally
 * equivalent C implementation is provided under `Src-C/`.
 *
 * Usage:
 *   - Include this header in your C++ source and construct `DEBUG` with a
 *     pointer to a configured `UART_HandleTypeDef`.
 *   - Call `log`, `info`, `error`, etc. to print messages.
 *
 * Notes:
 *   - Requires STM32Cube HAL UART module; enable `HAL_UART_MODULE_ENABLED`.
 *   - See repository README for examples and integration instructions.
 *
 * @author:    WilliTourt <willitourt@foxmail.com>
 * @date:      2025-12-10
 ******************************************************************************/

#pragma once

#include "main.h"

#include <cstdio>
#include <cstdarg>
#include <cstring>

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


/* ANSI escape codes for output *****************************************/

// Clear color and style of text before this macro
#define CLR                 "\033[0m"

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

// Background colors
#define BG_RED              "\033[41m"
#define BG_GREEN            "\033[42m"
#define BG_YELLOW           "\033[43m"
#define BG_BLUE             "\033[44m"

// Styles
#define BOLD                "\033[1m"
#define UNDERLINE           "\033[4m"
#define BLINK               "\033[5m"

// Prefixes
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

class DEBUG {
    public:

        // Constructor: can enable/disable timestamp and color output globally
        DEBUG(UART_HandleTypeDef *huart, bool enable_timestamp = true, bool enable_color = true);

        // Basic formatted log
        void log(const char* format, ...);

        // Log with a type prefix
        void logWithType(const char* type, const char* format, ...);

        // Convenience helpers
        void error(const char* format, ...);
        void warning(const char* format, ...);
        void ok(const char* format, ...);
        void success(const char* format, ...);
        void info(const char* format, ...);

        inline void setTimestampEnabled(bool enabled) { _timestamp_enabled = enabled; }
        inline void setColorEnabled(bool enabled) { _color_enabled = enabled; }

    private:
        UART_HandleTypeDef *_huart;
        bool _timestamp_enabled;
        bool _color_enabled;

        void _send(const char* text);
};
