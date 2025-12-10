#/*******************************************************************************
 * @file    debug.c
 * @brief   C implementation for ANSI-colored debug logging on STM32.
 *
 * Implements the C API declared in `Src-C/debug.h`. Provides formatted
 * logging functions that send output over a HAL UART interface. Supports
 * optional timestamps (using `HAL_GetTick()`) and ANSI color prefixes.
 *
 * Make sure to call `debug_init()` with a valid `UART_HandleTypeDef *`
 * before using other functions in this file.
 *
 * @author:    WilliTourt <willitourt@foxmail.com>
 * @date:      2025-12-10
 ******************************************************************************/

#include "debug.h"

static UART_HandleTypeDef *_huart = NULL;
static bool _timestamp_enabled = true;
static bool _color_enabled = true;

void debug_init(UART_HandleTypeDef *huart, bool enable_timestamp, bool enable_color) {
	_huart = huart;
	_timestamp_enabled = enable_timestamp;
	_color_enabled = enable_color;
}

static void _send(const char* text) {
	if (_huart == NULL || text == NULL) return;

	char out[DEBUG_BUFFER_LEN * 2];
	size_t pos = 0;

	if (_timestamp_enabled) {
		uint32_t ms = HAL_GetTick();
		uint32_t s = ms / 1000U;
		uint32_t hours = s / 3600U;
		uint32_t minutes = (s % 3600U) / 60U;
		uint32_t seconds = s % 60U;
		pos = snprintf(out, sizeof(out), "[%02lu:%02lu:%02lu.%03lu] ",
					   (unsigned long)hours, (unsigned long)minutes,
					   (unsigned long)seconds, (unsigned long)(ms % 1000U));
		if (pos >= sizeof(out)) pos = 0;
	}

	/* append text safely */
	if (pos < sizeof(out)) {
		size_t remain = sizeof(out) - pos;
		/* leave room for null terminator */
		strncpy(out + pos, text, (remain > 0) ? (remain - 1) : 0);
		out[sizeof(out) - 1] = '\0';
	}

	HAL_UART_Transmit(_huart, (uint8_t*)out, (uint16_t)strlen(out), HAL_MAX_DELAY);
}

void debug_log(const char* format, ...) {
	char msg[DEBUG_BUFFER_LEN];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);
	_send(msg);
}

void debug_logWithType(const char* type, const char* format, ...) {
	char msg[DEBUG_BUFFER_LEN];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	char combined[DEBUG_BUFFER_LEN + 32];
    snprintf(combined, sizeof(combined), "[ %s ] %s", type, msg);
	_send(combined);
}

void debug_error(const char* format, ...) {
	char msg[DEBUG_BUFFER_LEN];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	const char* prefix = _color_enabled ? ERROR_TYPE : ERROR_TYPE_PLAIN;
	char combined[DEBUG_BUFFER_LEN + 32];
	snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
	_send(combined);
}

void debug_warning(const char* format, ...) {
	char msg[DEBUG_BUFFER_LEN];
	va_list args;
	va_start(args, format);
	vsnprintf(msg, sizeof(msg), format, args);
	va_end(args);

	const char* prefix = _color_enabled ? WARNING_TYPE : WARNING_TYPE_PLAIN;
	char combined[DEBUG_BUFFER_LEN + 32];
	snprintf(combined, sizeof(combined), "%s%s", prefix, msg);
	_send(combined);
}

void debug_ok(const char* format, ...) {
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

void debug_success(const char* format, ...) {
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

void debug_info(const char* format, ...) {
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

void debug_setTimestampEnabled(bool enabled) {
	_timestamp_enabled = enabled;
}

void debug_setColorEnabled(bool enabled) {
	_color_enabled = enabled;
}
