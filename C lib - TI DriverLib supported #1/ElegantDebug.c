/*******************************************************************************
 * @file    ElegantDebug.c
 * @version 1.1-mspm0-tickptr
 * @brief   C implementation for ANSI-colored debug logging on TI MSPM0.
 *
 * UART send: DL_UART_Main_transmitData (byte-by-byte, blocking by TX FIFO full)
 * Timestamp: user-provided volatile uint32_t* tick pointer (ms)
 ******************************************************************************/

#include "ElegantDebug.h"

static UART_Regs * _uart_base = NULL;
static volatile uint32_t *_tick_ptr = NULL;

static bool _timestamp_enabled = true;
static bool _color_enabled = true;
static bool _filename_line_enabled = false;

/* ===== low-level UART send (blocking) ===== */
static void _uart_send_string(const char *str)
{
    if (_uart_base == 0 || str == NULL) return;

    while (*str) {
        while (DL_UART_Main_isTXFIFOFull(_uart_base)) {
            /* busy wait */
        }
        DL_UART_Main_transmitData(_uart_base, (uint8_t)(*str));
        str++;
    }
}

/* ===== internal send with optional timestamp ===== */
static void _send(const char* text)
{
    if (_uart_base == 0 || text == NULL) return;

    char out[DEBUG_BUFFER_LEN * 2];
    size_t pos = 0;

    if (_timestamp_enabled) {
        uint32_t ms = 0;
        if (_tick_ptr != NULL) {
            ms = *_tick_ptr;
        }

        uint32_t s = ms / 1000U;
        uint32_t hours = s / 3600U;
        uint32_t minutes = (s % 3600U) / 60U;
        uint32_t seconds = s % 60U;

        pos = (size_t)snprintf(out, sizeof(out),
                               "[%02lu:%02lu:%02lu.%03lu] ",
                               (unsigned long)hours,
                               (unsigned long)minutes,
                               (unsigned long)seconds,
                               (unsigned long)(ms % 1000U));
        if (pos >= sizeof(out)) pos = 0;
    }

    /* append text safely */
    if (pos < sizeof(out)) {
        size_t remain = sizeof(out) - pos;
        strncpy(out + pos, text, (remain > 0) ? (remain - 1) : 0);
        out[sizeof(out) - 1] = '\0';
    } else {
        out[sizeof(out) - 1] = '\0';
    }

    _uart_send_string(out);
}

void debug_init(UART_Regs * uart_base,
                volatile uint32_t *tick_ptr,
                bool enable_timestamp,
                bool enable_color,
                bool enable_filename_line)
{
    _uart_base = uart_base;
    _tick_ptr = tick_ptr;

    _timestamp_enabled = enable_timestamp;
    _color_enabled = enable_color;
    _filename_line_enabled = enable_filename_line;
}

void debug_log(const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);
    _send(msg);
}

void debug_logWithType(const char* type, const char* style, const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    char combined[DEBUG_BUFFER_LEN + 32];
    snprintf(combined, sizeof(combined),
             "\033[1m%s[%s]\033[0m %s",
             (style != NULL) ? style : "",
             (type  != NULL) ? type  : "",
             msg);
    _send(combined);
}

void debug_error_fileline(const char* file, int line, const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    const char* prefix = _color_enabled ? ERROR_TYPE : ERROR_TYPE_PLAIN;
    char combined[DEBUG_BUFFER_LEN + 96];

    if (_filename_line_enabled && file != NULL) {
        snprintf(combined, sizeof(combined), "%s[%s:%d] %s", prefix, file, line, msg);
    } else {
        snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
    }
    _send(combined);
}

void debug_warning_fileline(const char* file, int line, const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    const char* prefix = _color_enabled ? WARNING_TYPE : WARNING_TYPE_PLAIN;
    char combined[DEBUG_BUFFER_LEN + 96];

    if (_filename_line_enabled && file != NULL) {
        snprintf(combined, sizeof(combined), "%s[%s:%d] %s", prefix, file, line, msg);
    } else {
        snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
    }
    _send(combined);
}

void debug_ok(const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    const char* prefix = _color_enabled ? OK_TYPE : OK_TYPE_PLAIN;
    char combined[DEBUG_BUFFER_LEN + 32];
    snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
    _send(combined);
}

void debug_success(const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    const char* prefix = _color_enabled ? SUCCESS_TYPE : SUCCESS_TYPE_PLAIN;
    char combined[DEBUG_BUFFER_LEN + 32];
    snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
    _send(combined);
}

void debug_info(const char* format, ...)
{
    char msg[DEBUG_BUFFER_LEN];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    const char* prefix = _color_enabled ? INFO_TYPE : INFO_TYPE_PLAIN;
    char combined[DEBUG_BUFFER_LEN + 32];
    snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
    _send(combined);
}

void debug_setTimestampEnabled(bool enabled)
{
    _timestamp_enabled = enabled;
}

void debug_setColorEnabled(bool enabled)
{
    _color_enabled = enabled;
}

void debug_setFilenameLineEnabled(bool enabled)
{
    _filename_line_enabled = enabled;
}
